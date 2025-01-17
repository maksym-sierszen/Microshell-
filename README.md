# Microshell

## Overview
This project is a custom shell (microshell) created as part of the **Operating Systems** course at **Adam Mickiewicz University in Poznań**. The shell provides basic functionality to execute user commands, manage directories, and maintain command history.

---

## Features
- **Custom Command Prompt**: Displays the current working directory and user name.
- **Built-in Commands**:
  - `cd` - Change the working directory (supports `~` for the home directory).
  - `help` - Display information about available commands.
  - `exit` - Terminate the shell.
  - `history` - View or clear the command history (`history -c` to clear).
  - `date` - Display the current date and time.
- **Command Execution**: Supports execution of external programs.
- **Command History**: Saves and displays a history of executed commands in a persistent file (`history.txt`).

---

## How It Works
1. The shell starts by initializing the command history file path and enters an infinite loop.
2. In each iteration:
   - The current working directory and user are displayed as a prompt.
   - User input is collected and tokenized into individual arguments.
   - Built-in commands are executed directly within the shell.
   - External commands are executed in a child process using `fork()` and `execvp()`.
3. The shell also supports basic error handling for invalid commands or arguments.

---

## Built-in Command Descriptions
- **cd [path]**: Change the current directory. Supports paths starting with `~`.
- **help**: Displays usage instructions and available commands.
- **exit**: Exits the shell.
- **history**: Shows a list of previous commands. Use `history -c` to clear the history.
- **date**: Prints the current date and time.

---

## Usage

1. Clone the repository:
   ```bash
   git clone https://github.com/maksym-sierszen/sop-microshell.git
   cd sop-microshell
   ```

2. Compile the program:
   ```bash
   gcc microshell.c -o microshell
   ```

3. Run the shell:
   ```bash
   ./microshell
   ```
