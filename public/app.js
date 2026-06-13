// ============================================================
// AI Prompt Studio - Frontend Application Script
// ============================================================

// State Management
const state = {
    currentUser: null,
    prompts: [],
    stats: null,
    activeView: 'view-dashboard',
    currentEditingPrompt: null // null for new prompt, object for editing
};

// SVG Circumference for circular progress ring
const CIRCUMFERENCE = 314;

// DOM Elements
const elements = {
    // Screens
    authScreen: document.getElementById('auth-screen'),
    appScreen: document.getElementById('app-screen'),
    
    // Forms
    loginForm: document.getElementById('login-form'),
    registerForm: document.getElementById('register-form'),
    authAlert: document.getElementById('auth-alert'),
    toRegisterLink: document.getElementById('to-register'),
    toLoginLink: document.getElementById('to-login'),
    
    // Auth inputs
    loginUser: document.getElementById('login-user'),
    loginPass: document.getElementById('login-pass'),
    registerUser: document.getElementById('register-user'),
    registerPass: document.getElementById('register-pass'),
    
    // Navigation
    navItems: document.querySelectorAll('.nav-item'),
    currentUsername: document.getElementById('current-username'),
    btnLogout: document.getElementById('btn-logout'),
    btnNewPromptNav: document.getElementById('btn-new-prompt-nav'),
    viewTitle: document.getElementById('view-title'),
    
    // Views
    views: document.querySelectorAll('.content-view'),
    
    // Dashboard
    statTotal: document.getElementById('stat-total'),
    statAvg: document.getElementById('stat-avg'),
    statBest: document.getElementById('stat-best'),
    barCoding: document.getElementById('bar-coding'),
    barWriting: document.getElementById('bar-writing'),
    barResearch: document.getElementById('bar-research'),
    barImage: document.getElementById('bar-image'),
    searchInput: document.getElementById('search-input'),
    filterBtns: document.querySelectorAll('.filter-btn'),
    promptListContainer: document.getElementById('prompt-list-container'),
    
    // Analyzer
    editorPromptId: document.getElementById('editor-prompt-id'),
    editorTitle: document.getElementById('editor-title'),
    editorCategory: document.getElementById('editor-category'),
    editorFavorite: document.getElementById('editor-favorite'),
    editorContent: document.getElementById('editor-content'),
    btnSavePrompt: document.getElementById('btn-save-prompt'),
    btnEnhancePrompt: document.getElementById('btn-enhance-prompt'),
    btnClearEditor: document.getElementById('btn-clear-editor'),
    saveStatus: document.getElementById('save-status'),
    
    // Feedback Panel
    analysisScore: document.getElementById('analysis-score'),
    analysisRatingBadge: document.getElementById('analysis-rating-badge'),
    progressRingCircle: document.querySelector('.progress-ring-circle'),
    metricsGridContainer: document.getElementById('metrics-grid-container'),
    missingChipsContainer: document.getElementById('missing-chips-container'),
    tipsListContainer: document.getElementById('tips-list-container'),
    versionHistoryPanel: document.getElementById('version-history-panel'),
    versionTimelineContainer: document.getElementById('version-timeline-container'),
    
    // Compare View
    compareP1: document.getElementById('compare-p1'),
    compareP2: document.getElementById('compare-p2'),
    compareCol1: document.getElementById('compare-col-1'),
    compareCol2: document.getElementById('compare-col-2'),
    
    // Toast
    toast: document.getElementById('toast')
};

// ============================================================
// UTILITIES & DEBOUNCE
// ============================================================

function debounce(func, wait) {
    let timeout;
    return function(...args) {
        clearTimeout(timeout);
        timeout = setTimeout(() => func.apply(this, args), wait);
    };
}

function showToast(message, type = 'info') {
    elements.toast.textContent = message;
    elements.toast.className = `toast toast-${type} show`;
    
    // Add icon based on type
    const icon = document.createElement('i');
    if (type === 'success') icon.className = 'fa-solid fa-circle-check';
    else if (type === 'error') icon.className = 'fa-solid fa-circle-xmark';
    else icon.className = 'fa-solid fa-circle-info';
    elements.toast.prepend(icon);

    setTimeout(() => {
        elements.toast.classList.remove('show');
    }, 3500);
}

// ============================================================
// VIEW SWITCHER
// ============================================================

function switchView(viewId) {
    elements.views.forEach(view => {
        if (view.id === viewId) {
            view.classList.add('active');
        } else {
            view.classList.remove('active');
        }
    });

    elements.navItems.forEach(item => {
        if (item.dataset.target === viewId) {
            item.classList.add('active');
        } else {
            item.classList.remove('active');
        }
    });

    state.activeView = viewId;

    // Update Header Title
    if (viewId === 'view-dashboard') elements.viewTitle.textContent = 'Dashboard';
    else if (viewId === 'view-analyzer') {
        elements.viewTitle.textContent = state.currentEditingPrompt ? 'Edit Prompt' : 'New Prompt';
    }
    else if (viewId === 'view-compare') elements.viewTitle.textContent = 'Compare Prompts';
}

// ============================================================
// AUTHENTICATION FLOW
// ============================================================

// Check Session on startup
function initSession() {
    const savedUser = localStorage.getItem('username');
    if (savedUser) {
        state.currentUser = savedUser;
        elements.currentUsername.textContent = savedUser;
        elements.authScreen.classList.add('hidden');
        elements.appScreen.classList.remove('hidden');
        loadAllData();
    } else {
        elements.authScreen.classList.remove('hidden');
        elements.appScreen.classList.add('hidden');
    }
}

// Show Alert message in auth box
function showAuthAlert(message, type = 'error') {
    elements.authAlert.textContent = message;
    elements.authAlert.className = `alert alert-${type}`;
    elements.authAlert.classList.remove('hidden');
    setTimeout(() => {
        elements.authAlert.classList.add('hidden');
    }, 4000);
}

// ============================================================
// API CORE WRAPPER REQUESTS
// ============================================================

async function apiRequest(endpoint, method = 'GET', data = null) {
    const token = localStorage.getItem('session_token');
    const options = {
        method,
        headers: {
            'Content-Type': 'application/json',
            ...(token ? { 'Authorization': `Bearer ${token}` } : {})
        }
    };
    if (data) {
        options.body = JSON.stringify(data);
    }
    const response = await fetch(endpoint, options);
    
    if (response.status === 401) {
        // Session expired or invalid, log out
        localStorage.removeItem('username');
        localStorage.removeItem('session_token');
        elements.authScreen.classList.remove('hidden');
        elements.appScreen.classList.add('hidden');
        showToast('Session expired or unauthorized. Please log in again.', 'error');
        throw new Error('Unauthorized');
    }

    if (!response.ok) {
        const err = await response.json();
        throw new Error(err.error || 'API Request failed');
    }
    return response.json();
}

async function loadAllData() {
    try {
        await Promise.all([
            loadPromptsList(),
            loadDashboardStats()
        ]);
        populateCompareSelects();
    } catch (err) {
        showToast(err.message, 'error');
    }
}

async function loadPromptsList() {
    state.prompts = await apiRequest('/api/prompts');
    renderPromptsList();
}

async function loadDashboardStats() {
    state.stats = await apiRequest('/api/prompts/stats');
    renderStatsDashboard();
}

// ============================================================
// RENDER INTERFACES
// ============================================================

function getScoreBadgeClass(score) {
    if (score >= 4.5) return 'score-excellent';
    if (score >= 3.5) return 'score-good';
    if (score >= 2.5) return 'score-average';
    if (score >= 1.5) return 'score-weak';
    return 'score-poor';
}

function getRatingLabel(score) {
    if (score >= 4.5) return 'EXCELLENT';
    if (score >= 3.5) return 'GOOD';
    if (score >= 2.5) return 'AVERAGE';
    if (score >= 1.5) return 'WEAK';
    return 'POOR';
}

function renderPromptsList(filter = 'all', searchQuery = '') {
    elements.promptListContainer.innerHTML = '';
    
    let filtered = [...state.prompts];

    // Filter type
    if (filter === 'favorites') {
        filtered = filtered.filter(p => p.isFavorite);
    } else if (filter !== 'all') {
        filtered = filtered.filter(p => p.category === filter);
    }

    // Search text
    if (searchQuery.trim() !== '') {
        const query = searchQuery.toLowerCase();
        filtered = filtered.filter(p => 
            p.title.toLowerCase().includes(query) || 
            p.content.toLowerCase().includes(query)
        );
    }

    if (filtered.length === 0) {
        elements.promptListContainer.innerHTML = `
            <div class="empty-state glass">
                <i class="fa-solid fa-circle-exclamation"></i>
                <p>No prompts match the selected filter criteria.</p>
            </div>
        `;
        return;
    }

    filtered.forEach(p => {
        const card = document.createElement('div');
        card.className = `prompt-card glass cat-${p.category.toLowerCase()}`;
        
        const rating = getRatingLabel(p.overallScore);
        const ratingClass = getScoreBadgeClass(p.overallScore);

        card.innerHTML = `
            <div class="card-header">
                <h3 class="card-title">${escapeHTML(p.title)}</h3>
                <button class="btn-favorite ${p.isFavorite ? 'active' : ''}" data-id="${p.id}">
                    <i class="fa-solid fa-star"></i>
                </button>
            </div>
            <div class="card-meta">
                <span class="badge badge-cat">${escapeHTML(p.category)}</span>
                <span class="badge badge-score ${ratingClass}">${p.overallScore.toFixed(1)} / 5.0 (${rating})</span>
            </div>
            <div class="card-body">
                ${escapeHTML(p.content)}
            </div>
            <div class="card-footer">
                <span class="edits-count"><i class="fa-solid fa-clock-rotate-left"></i> ${p.editCount} edits</span>
                <div class="card-actions">
                    <button class="btn-card-action btn-card-edit" data-id="${p.id}" title="Edit prompt in Analyzer">
                        <i class="fa-solid fa-pen-to-square"></i>
                    </button>
                    <button class="btn-card-action btn-card-delete" data-id="${p.id}" title="Delete prompt">
                        <i class="fa-solid fa-trash-can"></i>
                    </button>
                </div>
            </div>
        `;

        // Card action listeners
        card.querySelector('.btn-favorite').addEventListener('click', async (e) => {
            e.stopPropagation();
            try {
                const updated = await apiRequest('/api/prompts/save', 'POST', {
                    id: p.id,
                    category: p.category,
                    title: p.title,
                    content: p.content,
                    isFavorite: !p.isFavorite
                });
                showToast(updated.isFavorite ? 'Added to favorites!' : 'Removed from favorites', 'info');
                loadAllData();
            } catch (err) {
                showToast(err.message, 'error');
            }
        });

        card.querySelector('.btn-card-edit').addEventListener('click', () => {
            loadPromptIntoEditor(p);
        });

        card.querySelector('.btn-card-delete').addEventListener('click', async () => {
            if (confirm(`Are you sure you want to delete prompt "${p.title}"?`)) {
                try {
                    const res = await apiRequest(`/api/prompts/${p.id}`, 'DELETE');
                    if (res.success) {
                        showToast('Prompt deleted successfully.', 'success');
                        loadAllData();
                    }
                } catch (err) {
                    showToast(err.message, 'error');
                }
            }
        });

        elements.promptListContainer.appendChild(card);
    });
}

function renderStatsDashboard() {
    if (!state.stats) return;
    
    elements.statTotal.textContent = state.stats.total;
    elements.statAvg.textContent = state.stats.total > 0 ? state.stats.avgScore.toFixed(1) : '0.0';
    elements.statBest.textContent = state.stats.best ? `${state.stats.best.overallScore.toFixed(1)}/5.0` : '-';
    
    // Calculate percentages for category breakdown bars
    const total = state.stats.total || 1;
    const codingPercent = ((state.stats.counts[0] / total) * 100).toFixed(0);
    const writingPercent = ((state.stats.counts[1] / total) * 100).toFixed(0);
    const researchPercent = ((state.stats.counts[2] / total) * 100).toFixed(0);
    const imagePercent = ((state.stats.counts[3] / total) * 100).toFixed(0);

    elements.barCoding.querySelector('.bar-inner').style.width = `${codingPercent}%`;
    elements.barCoding.querySelector('.count').textContent = `${state.stats.counts[0]} (${codingPercent}%)`;
    
    elements.barWriting.querySelector('.bar-inner').style.width = `${writingPercent}%`;
    elements.barWriting.querySelector('.count').textContent = `${state.stats.counts[1]} (${writingPercent}%)`;
    
    elements.barResearch.querySelector('.bar-inner').style.width = `${researchPercent}%`;
    elements.barResearch.querySelector('.count').textContent = `${state.stats.counts[2]} (${researchPercent}%)`;
    
    elements.barImage.querySelector('.bar-inner').style.width = `${imagePercent}%`;
    elements.barImage.querySelector('.count').textContent = `${state.stats.counts[3]} (${imagePercent}%)`;
}

// Helper to escape HTML tags to prevent XSS
function escapeHTML(str) {
    if (!str) return '';
    return str.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;").replace(/'/g, "&#039;");
}

// ============================================================
// REAL-TIME ANALYZER PANEL
// ============================================================

// Live evaluation request
const liveAnalyze = debounce(async () => {
    const category = elements.editorCategory.value;
    const title = elements.editorTitle.value || 'Untitled Analysis';
    const content = elements.editorContent.value;
    
    if (!content.trim()) {
        resetAnalysisFeedback();
        return;
    }

    elements.saveStatus.textContent = 'Analyzing...';

    try {
        const report = await apiRequest('/api/prompts/analyze', 'POST', {
            category,
            title,
            content
        });
        
        updateAnalysisFeedback(report);
        elements.saveStatus.textContent = state.currentEditingPrompt ? 'Modified' : 'Draft';
    } catch (err) {
        console.error(err);
        elements.saveStatus.textContent = 'Error';
    }
}, 550);

function updateAnalysisFeedback(report) {
    const score = report.prompt.overallScore;
    elements.analysisScore.textContent = score.toFixed(1);
    
    // Set ring stroke dashoffset
    const offset = CIRCUMFERENCE - (score / 5) * CIRCUMFERENCE;
    elements.progressRingCircle.style.strokeDashoffset = offset;
    
    // Rating class
    const ratingLabel = report.rating;
    elements.analysisRatingBadge.textContent = ratingLabel;
    elements.analysisRatingBadge.className = 'rating-label-box'; // reset
    if (score >= 4.5) elements.analysisRatingBadge.classList.add('rating-excellent');
    else if (score >= 3.5) elements.analysisRatingBadge.classList.add('rating-good');
    else if (score >= 2.5) elements.analysisRatingBadge.classList.add('rating-average');
    else if (score >= 1.5) elements.analysisRatingBadge.classList.add('rating-weak');
    else elements.analysisRatingBadge.classList.add('rating-poor');

    // Metrics details
    elements.metricsGridContainer.innerHTML = '';
    const cat = report.prompt.category;
    
    // Metrics Labels mapping
    let metricLabels = [];
    if (cat === 'Coding') {
        metricLabels = ['Word length', 'Target Language', 'Expected Output', 'Algorithm complexity', 'Edge Case handling'];
    } else if (cat === 'Writing') {
        metricLabels = ['Word length', 'Tone definition', 'Target Audience', 'Length specification', 'Deliverable format'];
    } else if (cat === 'Research') {
        metricLabels = ['Word length', 'Narrow Scope', 'Depth of analysis', 'Angle / Perspective', 'Citations / References'];
    } else if (cat === 'Image') {
        metricLabels = ['Word length', 'Art Style', 'Mood description', 'Color scheme', 'Lighting & Shot direction'];
    }

    report.prompt.scores.forEach((metricScore, idx) => {
        const card = document.createElement('div');
        card.className = 'metric-bar-card';
        card.innerHTML = `
            <div class="metric-header">
                <span>${metricLabels[idx]}</span>
                <span class="metric-val-indicator" style="color: ${metricScore === 5 ? 'var(--color-excellent)' : metricScore === 3 ? 'var(--color-average)' : 'var(--color-poor)'}">
                    ${metricScore === 5 ? 'EXCELLENT' : metricScore === 3 ? 'GOOD' : 'MISSING'}
                </span>
            </div>
            <div class="metric-bar-outer">
                <div class="metric-bar-inner val-${metricScore}"></div>
            </div>
        `;
        elements.metricsGridContainer.appendChild(card);
    });

    // Render Missing Items
    elements.missingChipsContainer.innerHTML = '';
    if (report.missing.length === 0) {
        elements.missingChipsContainer.innerHTML = `
            <span class="chip-excellent-state"><i class="fa-solid fa-circle-check"></i> Nothing! Excellent prompt.</span>
        `;
    } else {
        report.missing.forEach(missingText => {
            const chip = document.createElement('span');
            chip.className = 'chip-missing';
            chip.textContent = missingText;
            elements.missingChipsContainer.appendChild(chip);
        });
    }

    // Render Improvement Suggestions
    elements.tipsListContainer.innerHTML = '';
    report.tips.forEach(tip => {
        const item = document.createElement('li');
        item.textContent = tip;
        elements.tipsListContainer.appendChild(item);
    });
}

function resetAnalysisFeedback() {
    elements.analysisScore.textContent = '0.0';
    elements.progressRingCircle.style.strokeDashoffset = CIRCUMFERENCE;
    elements.analysisRatingBadge.textContent = 'WEAK';
    elements.analysisRatingBadge.className = 'rating-label-box rating-weak';
    elements.metricsGridContainer.innerHTML = '';
    elements.missingChipsContainer.innerHTML = '';
    elements.tipsListContainer.innerHTML = '<li>Provide details above to trigger calculations.</li>';
}

// Load prompt database object into editor
function loadPromptIntoEditor(p) {
    state.currentEditingPrompt = p;
    elements.editorPromptId.value = p.id;
    elements.editorTitle.value = p.title;
    elements.editorCategory.value = p.category;
    elements.editorFavorite.checked = p.isFavorite;
    elements.editorContent.value = p.content;

    elements.saveStatus.textContent = 'Saved';
    switchView('view-analyzer');
    
    // Trigger real-time calculation
    liveAnalyze();

    // Render Version History
    if (p.versionHistory && p.versionHistory.length > 0) {
        elements.versionHistoryPanel.classList.remove('hidden');
        elements.versionTimelineContainer.innerHTML = '';
        
        p.versionHistory.forEach((vText, idx) => {
            const item = document.createElement('div');
            item.className = 'timeline-item';
            item.innerHTML = `
                <div class="timeline-header">
                    <span>Version ${idx + 1}</span>
                    <span>Historical Iteration</span>
                </div>
                <div class="timeline-content">${escapeHTML(vText)}</div>
            `;
            elements.versionTimelineContainer.appendChild(item);
        });

        // Add current version at the top
        const currentItem = document.createElement('div');
        currentItem.className = 'timeline-item active';
        currentItem.innerHTML = `
            <div class="timeline-header">
                <span>Active Version (Current)</span>
                <span>Active</span>
            </div>
            <div class="timeline-content">${escapeHTML(p.content)}</div>
        `;
        elements.versionTimelineContainer.prepend(currentItem);
    } else {
        elements.versionHistoryPanel.classList.add('hidden');
    }
}

// Clear Editor fields
function clearEditor() {
    state.currentEditingPrompt = null;
    elements.editorPromptId.value = '0';
    elements.editorTitle.value = '';
    elements.editorCategory.value = 'Coding';
    elements.editorFavorite.checked = false;
    elements.editorContent.value = '';
    elements.saveStatus.textContent = 'Draft';
    elements.versionHistoryPanel.classList.add('hidden');
    resetAnalysisFeedback();
}

// Save Prompt
async function savePromptFromEditor() {
    const title = elements.editorTitle.value.trim();
    const category = elements.editorCategory.value;
    const isFavorite = elements.editorFavorite.checked;
    const content = elements.editorContent.value.trim();

    if (!title || !content) {
        showToast('Please fill in Title and Content before saving.', 'error');
        return;
    }

    try {
        const saved = await apiRequest('/api/prompts/save', 'POST', {
            id: parseInt(elements.editorPromptId.value) || 0,
            category,
            title,
            content,
            isFavorite
        });

        showToast(`Prompt "${saved.title}" saved successfully!`, 'success');
        clearEditor();
        switchView('view-dashboard');
        loadAllData();
    } catch (err) {
        showToast(err.message, 'error');
    }
}

// ============================================================
// COMPARE TOOL
// ============================================================

function populateCompareSelects() {
    // Save current selection to restore if list changes
    const sel1 = elements.compareP1.value;
    const sel2 = elements.compareP2.value;

    elements.compareP1.innerHTML = '<option value="">-- Choose Prompt A --</option>';
    elements.compareP2.innerHTML = '<option value="">-- Choose Prompt B --</option>';

    state.prompts.forEach(p => {
        const optA = document.createElement('option');
        optA.value = p.id;
        optA.textContent = `[ID: ${p.id}] ${p.title}`;
        elements.compareP1.appendChild(optA);

        const optB = document.createElement('option');
        optB.value = p.id;
        optB.textContent = `[ID: ${p.id}] ${p.title}`;
        elements.compareP2.appendChild(optB);
    });

    elements.compareP1.value = sel1;
    elements.compareP2.value = sel2;
}

function handleCompareChange(columnNum) {
    const select = columnNum === 1 ? elements.compareP1 : elements.compareP2;
    const container = columnNum === 1 ? elements.compareCol1 : elements.compareCol2;
    const idVal = select.value;

    if (!idVal) {
        container.innerHTML = `
            <div class="compare-placeholder">
                <i class="fa-solid fa-arrow-pointer"></i>
                <p>Select Prompt ${columnNum === 1 ? 'A' : 'B'} above to display details</p>
            </div>
        `;
        return;
    }

    const promptObj = state.prompts.find(p => p.id === parseInt(idVal));
    if (!promptObj) return;

    const rating = getRatingLabel(promptObj.overallScore);
    const scoreClass = getScoreBadgeClass(promptObj.overallScore);

    // Categories Metric headers
    const cat = promptObj.category;
    let labels = [];
    if (cat === 'Coding') {
        labels = ['Word length', 'Target Language', 'Expected Output', 'Algorithm complexity', 'Edge Case handling'];
    } else if (cat === 'Writing') {
        labels = ['Word length', 'Tone definition', 'Target Audience', 'Length specification', 'Deliverable format'];
    } else if (cat === 'Research') {
        labels = ['Word length', 'Narrow Scope', 'Depth of analysis', 'Angle / Perspective', 'Citations / References'];
    } else if (cat === 'Image') {
        labels = ['Word length', 'Art Style', 'Mood description', 'Color scheme', 'Lighting & Shot direction'];
    }

    let metricsHtml = '';
    promptObj.scores.forEach((s, idx) => {
        metricsHtml += `
            <div class="compare-metric-row">
                <span>${labels[idx]}</span>
                <span class="score-val" style="color: ${s === 5 ? 'var(--color-excellent)' : s === 3 ? 'var(--color-average)' : 'var(--color-poor)'}">
                    ${s === 5 ? 'EXCELLENT (5)' : s === 3 ? 'GOOD (3)' : 'MISSING (1)'}
                </span>
            </div>
        `;
    });

    container.innerHTML = `
        <div class="compare-details-header">
            <h3>${escapeHTML(promptObj.title)}</h3>
            <div class="compare-score-box">
                <span class="badge badge-cat">${escapeHTML(promptObj.category)}</span>
                <span class="badge badge-score ${scoreClass}">${promptObj.overallScore.toFixed(1)} (${rating})</span>
            </div>
        </div>
        
        <div class="form-group">
            <label>Prompt Body</label>
            <div class="compare-content-block">${escapeHTML(promptObj.content)}</div>
        </div>

        <div class="compare-metrics-list">
            <h4>Metric Evaluation</h4>
            ${metricsHtml}
        </div>

        <div class="compare-metric-row">
            <span>Historical Edits</span>
            <span class="score-val">${promptObj.editCount} revisions</span>
        </div>
    `;
}

// ============================================================
// EVENT LISTENERS REGISTER
// ============================================================

function registerEventListeners() {
    // Auth Switching Links
    elements.toRegisterLink.addEventListener('click', (e) => {
        e.preventDefault();
        elements.loginForm.classList.remove('active');
        elements.registerForm.classList.add('active');
    });

    elements.toLoginLink.addEventListener('click', (e) => {
        e.preventDefault();
        elements.registerForm.classList.remove('active');
        elements.loginForm.classList.add('active');
    });

    // Login Form Submit
    elements.loginForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        const username = elements.loginUser.value.trim();
        const password = elements.loginPass.value;

        if (!username || !password) return;

        try {
            const res = await apiRequest('/api/auth/login', 'POST', { username, password });
            if (res.success) {
                localStorage.setItem('username', res.username);
                localStorage.setItem('session_token', res.token);
                state.currentUser = res.username;
                elements.currentUsername.textContent = res.username;
                elements.authScreen.classList.add('hidden');
                elements.appScreen.classList.remove('hidden');
                elements.loginUser.value = '';
                elements.loginPass.value = '';
                loadAllData();
                showToast(`Welcome back, ${res.username}!`, 'success');
            } else {
                showAuthAlert(res.error || 'Login failed.');
            }
        } catch (err) {
            showAuthAlert(err.message);
        }
    });

    // Register Form Submit
    elements.registerForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        const username = elements.registerUser.value.trim();
        const password = elements.registerPass.value;

        if (!username || !password) return;

        try {
            const res = await apiRequest('/api/auth/register', 'POST', { username, password });
            if (res.success) {
                elements.registerUser.value = '';
                elements.registerPass.value = '';
                elements.registerForm.classList.remove('active');
                elements.loginForm.classList.add('active');
                showToast('Registration successful! Please login.', 'success');
            } else {
                showAuthAlert(res.error || 'Registration failed.');
            }
        } catch (err) {
            showAuthAlert(err.message);
        }
    });

    // Logout
    elements.btnLogout.addEventListener('click', () => {
        localStorage.removeItem('username');
        localStorage.removeItem('session_token');
        state.currentUser = null;
        clearEditor();
        elements.authScreen.classList.remove('hidden');
        elements.appScreen.classList.add('hidden');
        showToast('Logged out successfully.', 'info');
    });

    // Sidebar View Routing
    elements.navItems.forEach(item => {
        item.addEventListener('click', (e) => {
            e.preventDefault();
            switchView(item.dataset.target);
        });
    });

    // New Prompt Nav action
    elements.btnNewPromptNav.addEventListener('click', () => {
        clearEditor();
        switchView('view-analyzer');
    });

    // Dashboard Search filter
    elements.searchInput.addEventListener('input', () => {
        const activeFilterBtn = document.querySelector('.filter-btn.active');
        const filterVal = activeFilterBtn ? activeFilterBtn.dataset.filter : 'all';
        renderPromptsList(filterVal, elements.searchInput.value);
    });

    // Dashboard Category Buttons filter
    elements.filterBtns.forEach(btn => {
        btn.addEventListener('click', () => {
            elements.filterBtns.forEach(b => b.classList.remove('active'));
            btn.classList.add('active');
            renderPromptsList(btn.dataset.filter, elements.searchInput.value);
        });
    });

    // Real-time editor events
    elements.editorContent.addEventListener('input', liveAnalyze);
    elements.editorTitle.addEventListener('input', liveAnalyze);
    elements.editorCategory.addEventListener('change', liveAnalyze);

    // Save prompt submit
    elements.btnSavePrompt.addEventListener('click', savePromptFromEditor);
    
    // Enhance prompt structure
    elements.btnEnhancePrompt.addEventListener('click', async () => {
        const category = elements.editorCategory.value;
        const content = elements.editorContent.value.trim();
        const title = elements.editorTitle.value.trim() || 'Enhanced Prompt';

        if (!content) {
            showToast('Please type some prompt content first to enhance.', 'error');
            return;
        }

        elements.saveStatus.textContent = 'Enhancing...';
        
        try {
            const res = await apiRequest('/api/prompts/enhance', 'POST', {
                category,
                title,
                content
            });

            elements.editorContent.value = res.enhanced;
            showToast('Prompt structure optimized for 5.0 Excellent quality!', 'success');
            
            // Trigger C++ analysis update immediately
            liveAnalyze();
        } catch (err) {
            showToast(err.message, 'error');
            elements.saveStatus.textContent = 'Error';
        }
    });
    
    // Clear editor button
    elements.btnClearEditor.addEventListener('click', clearEditor);

    // Compare selectors change
    elements.compareP1.addEventListener('change', () => handleCompareChange(1));
    elements.compareP2.addEventListener('change', () => handleCompareChange(2));
}

// ============================================================
// ENTRY POINT INITIALIZER
// ============================================================

window.addEventListener('DOMContentLoaded', () => {
    registerEventListeners();
    initSession();
});
