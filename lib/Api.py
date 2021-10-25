#!/usr/bin/python3
# Send data to remote API
#   Penn Bauman <pcb8gb@virginia.edu>
import os
from urllib import parse, request, error


REMOTE_URL = "https://api.besic.org/device/data"


def sendData(mac, password, data):
    if type(data) is not dict:
        print("Invalid Data")
        return
    data_text = ""
    first = True
    for k, v in data.items():
        if first:
            data_text = str(k) + "=" + str(v)
            first = False
            continue
        data_text += "," + str(k) + "=" + str(v)

    encoded_data = parse.urlencode({
        "mac": mac,
        "password": password,
        "data": data_text,
    })
    encoded_data = encoded_data.encode('ascii')

    req = request.Request(REMOTE_URL, encoded_data)
    try:
        with request.urlopen(req) as res:
            res.read()
    except Exception as e:
        print(e)

def sendApiHeartbeat(lux=None,tmp=None,prs=None,hum=None):
    sendData(os.environ['MAC'], os.environ['PASSWORD'], {
        "lux": str(lux),
        "tmp": str(tmp),
        "prs": str(prs),
        "hum": str(hum),
    })


# Test with dummy device
if __name__ == "__main__":
    import random, sys
    REMOTE_URL = "http://localhost:3000/api/device/data"

    MAC = "000000000000"
    PSWD = "password"
    DATA = {
        "lux": "%.4f" % (random.random() * 10),
        "tmp": "%.4f" % (random.random() * 10),
        "prs": "%.4f" % (random.random() * 10),
        "hum": "%.4f" % (random.random() * 10),
    }
    print(MAC)
    for k, v in DATA.items():
        print("  " + k + ": " + v)
    sendData(MAC, PSWD, DATA)
