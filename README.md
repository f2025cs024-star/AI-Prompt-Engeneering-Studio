# AI Prompt Engineering Studio

An advanced, beginner-friendly full-stack web application designed to evaluate, optimize, and organize prompt templates for LLMs and AI agents. It wraps a robust C++ Object-Oriented Programming (OOP) rule-based analysis engine inside a modern, glassmorphic Node.js web interface.

---

## 🚀 Key Features

- **C++ Rules Engine**: Custom rule-based metric analysis evaluating prompts on word count, target language, expected output formatting, context details, and edge-case handling.
- **Auto-Enhance System**: Expand simple prompts instantly into robust, structured templates rated **5.0/5.0 (Excellent)** by the C++ engine.
- **Glassmorphic UI**: Beautiful, interactive front-end featuring dark mode, progress rings, radial indicators, and category distribution metrics.
- **Advanced Security**:
  - Hashed user credentials with SHA-256 (in both Node.js and C++ console mode).
  - Secure stateful sessions with custom token headers.
  - Strong HTTP security headers (CSP, X-Frame-Options, X-Content-Type-Options) to protect against Clickjacking, XSS, and MIME-sniffing.
- **Cross-Platform Compatibility**: Detection logic supporting Windows (`.exe` binary) and Linux/macOS cloud environments natively.

---

## 🛠️ Tech Stack & Architecture

```
   ┌────────────────────────────────────────────────────────┐
   │                  Web Frontend (HTML/CSS/JS)            │
   └───────────┬────────────────────────────────┬───────────┘
               │                                │
        (REST API Calls)                (Session Bearer Token)
               ▼                                ▼
   ┌────────────────────────────────────────────────────────┐
   │                  Node.js / Express Server               │
   └───────────────────────────┬────────────────────────────┘
                               │
                       (JSON IPC Bridge)
                               ▼
   ┌────────────────────────────────────────────────────────┐
   │                   C++ OOP Engine                       │
   │           (Polymorphism, Encapsulation,            │
   │           Inheritance, Abstraction)                    │
   └────────────────────────────────────────────────────────┘
```

1. **Frontend**: HTML5, Vanilla CSS3 (custom glassmorphic theme), and Vanilla Javascript (ES6+).
2. **Backend**: Node.js & Express.js.
3. **Engine**: C++11 console executable communicating via command line arguments and `stdin` JSON bridging.
4. **Database**: Lightweight file-based records (`prompts.txt` and `users.txt`).

---

## 📂 Project Structure

```
prompt-studio-web/
├── backend/
│   ├── prompt_studio.cpp   # Core C++ OOP rules engine
│   ├── prompt_studio.exe   # Compiled Windows executable
│   ├── server.js           # Node.js API server & session manager
│   ├── prompts.txt         # Local prompt storage database
│   └── users.txt           # Hashed credentials database
├── public/
│   ├── index.html          # Web dashboard layout
│   ├── style.css           # Custom glassmorphic styling
│   └── app.js              # Application state & API actions
├── .gitignore              # Git ignore rules
├── package.json            # Node project configuration
└── README.md               # Project documentation
```

---

## 🔒 Security Enhancements

1. **Password Hashing (SHA-256)**: All user passwords are dynamically hashed using a one-way cryptographic SHA-256 algorithm before storage in `users.txt`. Plaintext credentials are never saved.
2. **Session Guards**: Upon logging in, users receive a unique, server-generated session token. Secure APIs require this token as a `Bearer` header in the `Authorization` request, protecting user prompts from unauthorized REST calls.
3. **Clickjacking & XSS Shielding**:
   - `X-Frame-Options: DENY` prevents the application from being embedded in `<iframe>` tags on foreign sites.
   - `Content-Security-Policy` limits resource loading to trusted domains (Google Fonts, FontAwesome) and locks script executions to local assets.

---

## 💻 Local Setup & Execution

### Prerequisites
- Node.js (v14+)
- A C++ Compiler (GCC/g++ supporting C++11)

### Installation
1. Clone the repository and navigate to the folder:
   ```bash
   git clone <your-repository-url>
   cd prompt-studio-web
   ```
2. Install dependencies:
   ```bash
   npm install
   ```

### Compile & Run
1. Compile the C++ engine:
   * **Windows (MinGW/GCC)**:
     ```bash
     g++ -std=c++11 backend/prompt_studio.cpp -o backend/prompt_studio.exe
     ```
   * **Linux/macOS**:
     ```bash
     g++ -std=std=c++11 backend/prompt_studio.cpp -o backend/prompt_studio
     ```
2. Run the development server:
   ```bash
   npm run dev
   ```
3. Open `http://localhost:3000` in your web browser.

---

## ☁️ Cloud Deployment

This repository is ready for deployment on modern platforms like **Render**, **Railway**, or **Heroku**.

### Render.com Deployment Steps
1. Push your code to your GitHub repository.
2. Create a new **Web Service** on Render.
3. Link your GitHub repository.
4. Use the following configuration:
   - **Environment**: `Node`
   - **Build Command**: `npm install && npm run build` (This automatically runs the C++ compiler natively in Render's Linux environment)
   - **Start Command**: `npm start`
5. Click **Deploy Web Service**.

---

## 🎓 C++ OOP Concepts Showcase
- **Encapsulation**: Prompts and user details are represented as classes (`Prompt`, `User`, `LoginSystem`) with protected members and explicit getters/setters.
- **Inheritance**: `CodingPrompt` inherits from the base `Prompt` class, extending it with language and syntax requirements.
- **Polymorphism**: The `displayDetail` method is overridden in `CodingPrompt` using the `override` keyword to customize visualization.
- **Abstraction**: Complex evaluations and file operations are separated into dedicated interfaces/manager classes (`Analyzer`, `FileManager`, `Statistics`).
