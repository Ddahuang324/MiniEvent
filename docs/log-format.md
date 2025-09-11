# Log Format

The log format follows this structure:

`<ISO8601> <LEVEL> <COMPONENT> - <MESSAGE>`

- **ISO8601**: UTC timestamp (e.g., `2025-09-11T10:05:03Z`)
- **LEVEL**: Log level (TRACE, DEBUG, INFO, WARN, ERROR)
- **COMPONENT**: Source component (e.g., `EventBase`, `HTTPServer`)
- **MESSAGE**: Log message

Example:
`2025-09-11T10:05:03Z INFO EventBase - loop started`
