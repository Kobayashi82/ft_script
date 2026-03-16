<div align="center">

![WIP](https://img.shields.io/badge/work%20in%20progress-yellow?style=for-the-badge)
![System & Kernel](https://img.shields.io/badge/System-brown?style=for-the-badge)
![Terminal](https://img.shields.io/badge/Terminal-PTY-blue?style=for-the-badge)
![C Language](https://img.shields.io/badge/Language-C-red?style=for-the-badge)

*A reimplementation of the Unix script command*

</div>

<div align="center">
  <img src="/images/W_ft_script.jpg">
</div>

# ft_script

[README en Espanol](README_es.md)

`ft_script` is a reimplementation of the Unix `script` utility.

It records terminal sessions through a PTY and stores:
- output logs (default)
- optional input logs
- optional timing metadata for replay/analysis

The project is built in C with low-level Unix APIs and minimal parsing logic.

## 🔧 Installation

```bash
git clone https://github.com/Kobayashi82/ft_script.git
cd ft_script
make
```

## 🖥️ Usage

```bash
./ft_script [options] [file]
```

If no file is provided, output is written to `typescript`.

### Quick Usage

```bash
# Interactive session, default output file: typescript
./ft_script

# Interactive session to a custom file
./ft_script session.log

# Run a command and exit
./ft_script -c "ls -la"
```

### Options
| Option         | Description                                 |
|----------------|---------------------------------------------|
| `-I <file>`    | Log stdin to file                           |
| `-O <file>`    | Log stdout to file (default behavior)       |
| `-B <file>`    | Log stdin and stdout to the same file       |
| `-T <file>`    | Log timing information to file              |
| `-m <name>`    | Force format: `classic` or `advanced`       |
| `-a`           | Append to log file(s) instead of truncating |
| `-c <command>` | Run command instead of interactive shell    |
| `-e`           | Return child process exit code              |
| `-f`           | Flush after each write                      |
| `-E <when>`    | Echo input: `auto`, `always`, or `never`    |
| `-o <size>`    | Stop when output files exceed size          |
| `-q`           | Quiet mode (less terminal output)           |
| `-h`           | Show help                                   |
| `-V`           | Show version                                |
|

### Log Formats

- `advanced` (default): modern format with richer timing metadata
- `classic`: classic timing format compatibility

### Examples

```bash
# Log both input and output to one file
./ft_script -B session.log

# Save output + timing in classic mode
./ft_script -O out.log -T timing.log -m classic

# Append and flush immediately after writes
./ft_script -a -f -O out.log

# Return command exit code
./ft_script -e -c "grep hello missing_file"
```

### Notes

- Only single flags are supported (`-a`, not grouped short options like `-abc`).
- Designed mainly for interactive terminal sessions.
- Full behavior details and limitations are available in `./ft_script -h`.

## 📄 License

This project is licensed under the WTFPL – [Do What the Fuck You Want to Public License](http://www.wtfpl.net/about/).

---

<div align="center">

**📂 Developed as part of the 42 School curriculum 📂**

*"Everything you typed, faithfully recorded... whether you meant to or not"*

</div>
