# Elevatedrv

This project is just for learning purposes. It's a simple driver / usermode app combo that, once the driver is installed, elevates the current process to SYSTEM.


## Install Driver

```
bcedit /set testsigning on
bcdedit.exe /set nointegritychecks on
sc create kelevate type= kernel binPath= C:\dev\kelevate.sys
sc start kelevate
```

## Usage

```
elevate.exe <PID>
```
