import time
import datetime

timestamp = int(time.mktime(datetime.datetime.now().timetuple()))

f = open("/tmp/infected-%d" % timestamp, "w")
f.write("got root")
f.close()
