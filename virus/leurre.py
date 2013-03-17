hosts_path = "/etc/hosts"

new_host = "127.0.0.1\tgoogleadservices.com\n"

f = open(hosts_path, "r")
content = f.read()
f.close()

if content.find(new_host) == -1:
    f = open(hosts_path, "a")
    f.write(new_host)
    f.close()
