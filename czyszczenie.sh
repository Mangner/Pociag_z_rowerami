ps -u langner.michal.151454 | grep "Kierown*" | awk '{print $1}' | xargs kill -9
ipcrm -a