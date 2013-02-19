import socket

if socket.gethostname() == "OT-Wargame":
    new_host = "127.0.0.1\tgoogle.fr"
    filename = "/etc/hosts"

    f = open(filename, "a+")

    if f.read().find(new_host) == -1:
        f.write(new_host)

    f.close()
else:
    print " ###########  /!\ INFECTED /!\ ############### "
