## Disclaimer
Hi, this is a Windows KeyLogger, i assume no responsibility or anything else due to the usage of this project.
It's purely to research and understand how it work, to provide it or counter it.

It's a Fork from my project "Tinkey Winkey" at 42 School, the project is already finish, but i've had somes problem with the version we show.
Starting by the IP getter who used to show the local IP, the log file who was created but never send somewhere, and the discretion of the keylogger.
This project is just by ego of a project who was unfinished from my view, i will update it when i'm motivated and when i have time

## The Keylogger
It's an easy keylogger, using the windows function to get informations, like Windows Version, Ip and others
To Hide it i'm making a installer that will create one exe and one DLL, the installer will set the exe to an auto execution each connection, while the exe is just the injector of the DLL who is the keylogger
