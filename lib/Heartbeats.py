#!/usr/bin/python3
from urllib import parse, request

URL:str = "http://remote.besic.org/heartbeat/"


def sendHeartBeat(deviceID,deployment,lux=None,tmp=None,prs=None,hum=None,bs=False):
    """Sends measurements to server for remote monitoring

    Args:
        deviceID (str): Relay ID
        deployment (str): Associated Deployment
        lux (float): Light intenstiy value
        tmp (float): Temperature value in deg C
        prs (float): Pressure value in Pa
        hum (float): Humidity value in %
    """
    data:dict = {
        "DeviceID" : deviceID,
        "DeploymentID" : deployment,
        "LUX" : lux,
        "TMP" : tmp,
        "PRS" : prs,
        "HUM" : hum
    }

    encodeddata = parse.urlencode(data)
    encodeddata = encodeddata.encode('ascii')

    req = request.Request(URL+('bs'if bs else 'relay'),encodeddata)
    try:
        with request.urlopen(req) as res:
            res.read()
    except Exception as e:
        print(e)

if __name__ == "__main__": 
    try:
        import config as config
    except:
        import lib.config as config
    
    _vars = config.get()
    deployment = _vars['DEPLOYMENT']
    deviceID = _vars['ID']
    sendHeartBeat(deviceID,deployment,bs=True)
