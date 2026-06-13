const express = require('express');
const { spawn } = require('child_process');
const path = require('path');
const fs = require('fs');
const crypto = require('crypto');

const app = express();
const PORT = process.env.PORT || 3000;

// Password Hashing Helper
function hashPassword(password) {
    return crypto.createHash('sha256').update(password).digest('hex');
}

// Session Store (in-memory)
const activeSessions = new Map(); // token -> { username, createdAt }

// Cleanup sessions older than 24 hours every hour
setInterval(() => {
    const now = Date.now();
    for (const [token, session] of activeSessions.entries()) {
        if (now - session.createdAt > 24 * 60 * 60 * 1000) {
            activeSessions.delete(token);
        }
    }
}, 60 * 60 * 1000);

// Users database migration on startup
function migrateUsersDatabase() {
    const usersFile = path.join(__dirname, 'users.txt');
    if (!fs.existsSync(usersFile)) {
        const defaultAdminHash = hashPassword('admin123');
        fs.writeFileSync(usersFile, `admin|${defaultAdminHash}\n`, 'utf8');
        return;
    }
    try {
        const content = fs.readFileSync(usersFile, 'utf8');
        const lines = content.split('\n');
        let updated = false;
        const newLines = lines.map(line => {
            if (!line.trim()) return line;
            const parts = line.split('|');
            if (parts.length >= 2) {
                const username = parts[0];
                const password = parts[1].trim();
                const isHashed = /^[0-9a-fA-F]{64}$/.test(password);
                if (!isHashed) {
                    updated = true;
                    return `${username}|${hashPassword(password)}`;
                }
            }
            return line;
        });
        if (updated) {
            fs.writeFileSync(usersFile, newLines.join('\n'), 'utf8');
            console.log('[*] Database migration: plaintext passwords migrated to SHA-256 hashes.');
        }
    } catch (err) {
        console.error('[-] Error running users database migration:', err);
    }
}
migrateUsersDatabase();

// HTTP Security Headers Middleware
app.use((req, res, next) => {
    res.setHeader('X-Frame-Options', 'DENY');
    res.setHeader('X-Content-Type-Options', 'nosniff');
    res.setHeader('X-XSS-Protection', '1; mode=block');
    res.setHeader(
        'Content-Security-Policy',
        "default-src 'self'; style-src 'self' 'unsafe-inline' https://fonts.googleapis.com https://cdnjs.cloudflare.com; font-src 'self' https://fonts.gstatic.com https://cdnjs.cloudflare.com; script-src 'self' 'unsafe-inline'; img-src 'self' data:; connect-src 'self'"
    );
    next();
});

// Middleware
app.use(express.json());
app.use(express.static(path.join(__dirname, '..', 'public')));

// Auth Guard Middleware
function requireAuth(req, res, next) {
    const authHeader = req.headers['authorization'];
    if (!authHeader || !authHeader.startsWith('Bearer ')) {
        return res.status(401).json({ error: 'Authentication required.' });
    }
    const token = authHeader.substring(7);
    const session = activeSessions.get(token);
    if (!session) {
        return res.status(401).json({ error: 'Session expired or invalid.' });
    }
    session.createdAt = Date.now(); // Refresh session
    req.username = session.username;
    next();
}

// Helper to run C++ Executable (OS-aware)
function runCppEngine(args, stdinInput = null) {
    return new Promise((resolve, reject) => {
        const isWin = process.platform === 'win32';
        const exeName = isWin ? 'prompt_studio.exe' : 'prompt_studio';
        const exePath = path.join(__dirname, exeName);
        
        // Ensure C++ exe exists
        if (!fs.existsSync(exePath)) {
            return reject(new Error(`C++ engine executable not found (${exeName}). Please compile it.`));
        }

        // On Linux/macOS, spawn needs the absolute path to execute a binary
        const child = spawn(exePath, args, { cwd: __dirname });

        let stdout = '';
        let stderr = '';

        child.stdout.on('data', (data) => {
            stdout += data.toString();
        });

        child.stderr.on('data', (data) => {
            stderr += data.toString();
        });

        child.on('close', (code) => {
            if (code !== 0) {
                return reject(new Error(`C++ Engine exited with code ${code}. Stderr: ${stderr}`));
            }
            const output = stdout.trim();
            try {
                const parsed = JSON.parse(output);
                resolve(parsed);
            } catch (err) {
                // Return string if not standard JSON
                resolve(output);
            }
        });

        child.on('error', (err) => {
            reject(err);
        });

        if (stdinInput !== null) {
            child.stdin.write(stdinInput);
            child.stdin.end();
        }
    });
}

// REST Endpoints

// 1. Login
app.post('/api/auth/login', async (req, res) => {
    const { username, password } = req.body;
    if (!username || !password) {
        return res.status(400).json({ error: 'Username and password required.' });
    }
    try {
        const hashedPassword = hashPassword(password);
        const result = await runCppEngine(['--api-login', username, hashedPassword]);
        if (result.success) {
            const token = crypto.randomBytes(32).toString('hex');
            activeSessions.set(token, {
                username,
                createdAt: Date.now()
            });
            return res.json({ success: true, username: result.username, token });
        }
        res.json(result);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// 2. Register
app.post('/api/auth/register', async (req, res) => {
    const { username, password } = req.body;
    if (!username || !password) {
        return res.status(400).json({ error: 'Username and password required.' });
    }
    try {
        const hashedPassword = hashPassword(password);
        const result = await runCppEngine(['--api-register', username, hashedPassword]);
        res.json(result);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// 3. List all prompts (protected)
app.get('/api/prompts', requireAuth, async (req, res) => {
    try {
        const result = await runCppEngine(['--api-list']);
        res.json(result);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// 4. Save (create or update) prompt (protected)
app.post('/api/prompts/save', requireAuth, async (req, res) => {
    const { id, category, title, content, isFavorite } = req.body;
    if (!category || !title || !content) {
        return res.status(400).json({ error: 'Missing required fields.' });
    }

    // Format stdin payload:
    // Line 1: ID
    // Line 2: Category
    // Line 3: Title
    // Line 4: IsFavorite (1 or 0)
    // Remaining: Content
    const stdinPayload = `${id || 0}\n${category}\n${title}\n${isFavorite ? '1' : '0'}\n${content}`;

    try {
        const result = await runCppEngine(['--api-save'], stdinPayload);
        res.json(result);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// 5. Delete prompt (protected)
app.delete('/api/prompts/:id', requireAuth, async (req, res) => {
    const id = req.params.id;
    try {
        const result = await runCppEngine(['--api-delete', id]);
        res.json(result);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// 6. Stats Dashboard (protected)
app.get('/api/prompts/stats', requireAuth, async (req, res) => {
    try {
        const result = await runCppEngine(['--api-stats']);
        res.json(result);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// 7. Analyze prompt (no save, just analyze live - protected)
app.post('/api/prompts/analyze', requireAuth, async (req, res) => {
    const { category, title, content } = req.body;
    if (!category || !title || !content) {
        return res.status(400).json({ error: 'Category, Title, and Content are required.' });
    }

    // Format stdin payload:
    // Line 1: Category
    // Line 2: Title
    // Remaining: Content
    const stdinPayload = `${category}\n${title}\n${content}`;

    try {
        const result = await runCppEngine(['--api-analyze'], stdinPayload);
        res.json(result);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// Helper to expand and enhance prompt to make it excellent (5.0)
function enhancePromptText(category, title, content) {
    const trimmed = content.trim();
    if (category === 'Coding') {
        return `# Role
Act as an expert C++ and Python Software Engineer.

# Objective
${trimmed}

# Specifications & Requirements
- Target Language: Implement the core logic in C++ or Python (modern standards).
- Algorithm details: Use an efficient search, sort, loop, or recursion structure to optimize performance.
- Output Format: Ensure the code demonstrates how to return or print the output clearly.
- Error Handling: Handle boundary edge cases gracefully, such as empty collections, invalid inputs, or null structures.`;
    } else if (category === 'Writing') {
        return `# Role
Act as an expert Content Creator and Editor.

# Objective
${trimmed}

# Formatting & Style Guidelines
- Tone: Maintain a professional, persuasive, and engaging tone throughout.
- Target Audience: Write specifically for beginners, students, or general readers.
- Length: Limit the response to a brief overview, approximately 3 detailed paragraphs (under 500 words).
- Format: Deliver the output in the format of a structured essay, blog post, report, or story.`;
    } else if (category === 'Research') {
        return `# Role
Act as a Senior Academic Researcher and Analyst.

# Objective
${trimmed}

# Scope & Methodology
- Scope: Focus specifically on key areas and limit the scope to essential aspects regarding this topic.
- Depth: Provide an in-depth, detailed analysis and academic overview.
- Perspective: Contrast the pros and cons, detailing the historical cause and future impact.
- Citations: Request references, citing high-quality journal articles, books, or sources where applicable.`;
    } else if (category === 'Image') {
        return `# Style & Composition
Subject: ${trimmed}

# Creative Directives
- Art Style: Render this in a detailed digital painting, realistic 3d style, or detailed sketch.
- Mood & Atmosphere: Establish a moody, warm, and peaceful atmosphere.
- Color Palette: Highlight colorful, golden, and vibrant tones (e.g. blue and gold).
- Camera & Lighting: Compose with cinematic lighting, sunlight casting soft shadows, and a wide portrait angle.`;
    }
    return trimmed;
}

// 8. Enhance prompt (protected)
app.post('/api/prompts/enhance', requireAuth, (req, res) => {
    const { category, title, content } = req.body;
    if (!category || !content) {
        return res.status(400).json({ error: 'Category and Content are required.' });
    }
    const enhanced = enhancePromptText(category, title, content);
    res.json({ enhanced });
});

// Catch-all to serve index.html for SPA routes (if any)
app.get('*', (req, res) => {
    res.sendFile(path.join(__dirname, '..', 'public', 'index.html'));
});

// Start Server
app.listen(PORT, () => {
    console.log(`[+] AI Prompt Studio server is running live on http://localhost:${PORT}`);
});
