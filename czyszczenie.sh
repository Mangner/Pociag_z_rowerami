ps -u langner.michal.151454 | grep "Pasa*" | awk '{print $1}' | xargs kill -9
ps -u langner.michal.151454 | grep "Kier*" | awk '{print $1}' | xargs kill -9
ps -u langner.michal.151454 | grep "Zaw*" | awk '{print $1}' | xargs kill -9
ipcrm -a
