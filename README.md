******🎮 Xonix Game (DSA Project in C++)******

A feature-rich, 2D multiplayer game developed as part of a Data Structures & Algorithms course project. Built using C++ and SFML (Simple and Fast Multimedia Library), this modern take on the classic Xonix game showcases the practical implementation of key data structures and algorithms to deliver a responsive and interactive gaming experience.


******🚀 Features******

🔐 Login & Signup System — File-based secure authentication

🧭 Interactive Menus — Main menu, inventory, matchmaking, and more with smooth UI

🧑‍🤝‍🧑 Player Profiles — View friends, requests, and match history

🧑‍🔧 Multiplayer Mode — Real-time PvP with matchmaking and scoring

🧵 Matchmaking System — Priority queue-based pairing of players by skill

🏆 Leaderboard — Top 10 players tracked using a min heap

🎨 Inventory System — Themes and sounds managed via AVL Trees

💾 Save/Load System — Persistent single-player game state management

⏯️ Pause/Restart — In-game control for better experience



******🛠️ Technologies Used******

Language: C++

Graphics: SFML

File Handling: C++ fstream

Data Structures:

Min Heap — Leaderboard

Priority Queue — Matchmaking

AVL Tree — Inventory management

Linked List — Friends and saved tiles

Hash Table — Fast user lookup

Dynamic Arrays — Player and profile storage



******📂 Project Structure******

menu.cpp — Menu and UI navigation

player.cpp — Player profiles, leaderboard, authentication

matchmaking.cpp — Matchmaking system using priority queues

inventory.cpp — Theme and sound inventory (AVL Trees)

save.cpp — Game save/load system

game.cpp — Core gameplay logic and rendering



******👥 Team Members******

Shazad Ahmad — Inventory, Multiplayer, Save/Load, Matchmaking

Muhammad Haris — Menus, Leaderboard, Authentication, Profiles



******📆 Development Timeline******

UI/UX & Menus

Core Game Mechanics (Single/Multiplayer)

Authentication & Profiles

Leaderboard & Matchmaking

Inventory, Save/Load, and Final Integration



******⚔️ Challenges Solved******

Slow Matchmaking I/O → Optimized file writes

Leaderboard Inefficiency → Switched to a Min Heap

Unbalanced Matches → Implemented score-based priority queue

UI Lag → Reduced redraws using boolean flags







******🧠 Learnings******

This project enhanced our understanding of:

Real-world applications of DSA

File-based state persistence

UI design in SFML

Optimizing performance using the right data structures



******📜 License******

This project is developed for academic purposes. Feel free to fork and expand!
