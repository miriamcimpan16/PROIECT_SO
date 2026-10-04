# City Manager Project - Technical Summary

City Manager-> is a multi-process UNIX system application developed in C designed to process urban reports, monitor system events, and route inter-process communications securely.

## Key Features & Architecture

* **Custom Binary Storage & Access Control**: Manages urban reports stored in binary format, applying standard POSIX permissions and metadata checks (`stat`, `chmod`).
* **Process Automation**: Utilizes `fork()` and robust UNIX signal handling (`sigaction`) to orchestrate child processes and manage asynchronous events.
* **Data-Routing Hub**: Implements a communication pipeline via `pipe()` and `dup2()` to stream real-time operational statistics and logs between components (`city_hub`, `city_manager`, `monitor_reports`, and `scorer`).
* **Advanced Query & Filtering**: Features modular condition parsing (`parse_condition`) and condition matching (`match_condition`) to filter binary report structures dynamically based on parameters like severity, category, inspector, and timestamps.
