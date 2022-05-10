############################################################
# IOT Poject
# GUI for Smart Lock
# This program is the core that loads other dependent
# programs.
############################################################

import subprocess
import os
import signal
import Tkinter as SecureSmartLock
import ImageTk
import Image
import socket
import select
import httplib
import json
import hmac
import hashlib
import datetime
import sys
import time
from _ctypes import sizeof
from threading import Thread


# Constants
PORT_STATUS = 50013
HOST = "127.0.0.1"
baseURL = "api.parse.com"
APP_ID = "y9Ofck9rx4YBItxCHplu9fn0yAiATL7UuqmvdVSi"
REST_ID = "UICnImykZfpdn4RUfYfbRnVRwYZrQbrFnfzPIbEE"

# Initializing variables and basic set up for lock GUI. All images for the lock are pulled from ../Images/ folder
lockRoot = SecureSmartLock.Tk() 
imgLocked = ImageTk.PhotoImage(Image.open("../Images/locked.png").resize((100,100), Image.ANTIALIAS))


lockRoot.geometry("320x600")
lockRoot.title("Secure Lock Emulator")

# Define variables
# global username, password, authenticated, fixed_challenge
username = SecureSmartLock.StringVar()
password = SecureSmartLock.StringVar()
newUsername = SecureSmartLock.StringVar()
newPassword = SecureSmartLock.StringVar()
validFor = SecureSmartLock.StringVar()
count = 3
authenticated = 0
isValid = 0
lockStatus = None
currentTime = None
masterUser = False
tempUserFlag = False
new_path = "locked.png"
usernameList = ['Wei','Vera','Salar']
passwordList = ['iotWei','iotVera','iotSalar']

# Login button is clicked - save contents into variables
def loginButtonClick():
        global username, password
        # Save username and password entries
        username = usernameEntry.get()
        password = passwordEntry.get()
        #print "%s" %username
        #print "%s" %password

        # Authenticate user
	checkList(username, password)
	if authenticated == 1:
        	authenticate()
        updateParse("POST", "FrontDoor")

def checkList(username, password):
	global usernameList, passwordList, count, lockStatus, masterUser, authenticated
	if username not in usernameList:
		i = -1
	else:
		i = usernameList.index(username)

	if password not in passwordList:
		j = -1
	else:
		j = passwordList.index(password)

	if i == j and i != -1:
		print "valid username and password"
		if i<3:
			masterUser = True
		else:
			masterUser = False
		authenticated = 1
		
	elif count == 1:
		count = 3
		lockStatus = 2
		masterUser = False
		authenticated = 0
	else:
		count = count - 1
		lockStatus = 0
		masterUser = False
		authenticated = 0
		

def unlock():
	global lockstatus, isValid
	print"valid user!"
	isValid = 1

def failToUnlock():
	global lockStatus, isValid
	print"invalid user!"
	isValid = 0
def updateParse(method, parseClass):
        global username, APP_ID, REST_ID, baseURL
        URL = "/1/classes/%s" %parseClass
        connection = httplib.HTTPSConnection(baseURL, 443)
        connection.connect()
        connection.request(method, URL, json.dumps({
                "User": username,
        }), {
                "X-Parse-Application-Id": APP_ID,
                "X-Parse-REST-API-Key": REST_ID,
                "Content-Type": "application/json"
        })
        #result = json.loads(connection.getresponse().read())
        #print result

# Add new user: only valid with master Users
def addUser():
	global usernameList, passwordList, masterUser, currentTime, validFor, tempUserFlag
	newUsername = newUsernameEntry.get()
	newPassword = newPasswordEntry.get()
	validFor = validForEntry.get()
	currentTime = datetime.datetime.time(datetime.datetime.now())
	if masterUser == True:
		usernameList.append(newUsername)
		passwordList.append(newPassword)
		tempUserFlag = True;
		print "new username and passworde added"
	else:
		print "no master user detected"

# Unlock button is pressed
def unlockButtonClick():
        global authenticated, lockStatus, isValid
        print "Unlock Button Pressed"

        if authenticated and isValid:
                # Send unlock command to lock
		authenticated = 0
		isValid = 0
		lockStatus = 1
                #clientSendSocket(PORT_STATUS, "unlock")
		
        else:
                print "Error: User is not authorized. Please login."


# Lock button is pressed
def lockButtonClick():
        global authenticated, lockStatus, isValid

        print "Lock Button Pressed"
        if authenticated and isValid:
		authenticated = 0
		isValid = 0
		lockStatus = 0
                # Send lock command to lock
                #clientSendSocket(PORT_STATUS, "lock")
        else:
                print "Error: User is not authorized. Please login."



# This function is a callback after periodic interval. Called by lockRoot.after()
# We also update the GUI of lock here
def checkCondition():
	global masterUser, usernameList, passwordList, new_path, tempUserFlag
	if tempUserFlag == True:
		t = datetime.datetime.time(datetime.datetime.now())
		delta = (t.hour-currentTime.hour)*60+t.minute-currentTime.minute
		if delta > int(validFor):
			tempUserFlag = False
			usernameList.pop()
			passwordList.pop()
			print "temporary user deleted"

	print "Lock status is", lockStatus , type(lockStatus)

	if lockStatus == 0:
		new_path = "locked.png"
	if lockStatus == 1:
		new_path = "unlocked.png"
        if lockStatus == 2:
                new_path = "alert.png"

	if lockStatus != None:
		# Update the new image and text when needed in GUI
		newImg = ImageTk.PhotoImage(Image.open("../Images/" + new_path).resize((100,100), Image.ANTIALIAS))
		statusImg.configure(image = newImg)
		statusImg.image = newImg
		lockRoot.update()
                # Update text status on GUI

	lockRoot.after(5000, checkCondition)

# Client Side Socket Code. We establish socket connectivity with an endpoint and send weather and local information
# Exceptions need to be caught, else we will terminate abruptly and create havoc.
def clientSendSocket(port, data):
	socketLock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	socketLock.settimeout(2)
	try:
		socketLock.connect((HOST, port))
		socketLock.send(data)
	except socket.error:
		#s.close()
		print "lock::clientSendSocket(): Socket Exception"
	except error:
		#s.close()
		print "lock::clientSenseSocket(): Exception"
		pass
	socketLock.settimeout(None)
  	socketLock.close()


# Read data from TCP server
def clientReadSocket(port):
        socketLock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        socketLock.settimeout(2)
        try:
                socketLock.connect((HOST,port))
                socketLock.recv()
        except socket.error:
                print "lock::clientReadSocket(): Socket Exception"
        except error:
                print "lock::clientReadSocket(): Exception"
                pass
        socketLock.settimeout(None)
        socketLock.close()

def unlock():
	global authenticated, isValid
	authenticated = 1
	isValid = 1
	print authenticated
	print isValid

def FailToUnlock():
	global authenticated, isValid
	authenticated = 0
	isValid = 0
	print authenticated
	print isValid

def authenticate():
	global authenticated
	fixed_challenge='sfe45f3de2de4fr5ws3de4fre3w3swd3'
	rn='324bk32bl443jblesefeseferfeeaew2'
	intermediakey='42d9fbf6e602c3fc45edcd262497a51d9fcccaef2f9d655c00b09aaead7d5d54'
	loopIndex=True
# Create a TCP/IP socket
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Connect the socket to the port where the server is listening
	server_address = ('160.39.221.145', 6666)
	print 'connecting to %s port %s' % server_address
	sock.connect(server_address)
   # sock.sendall(askmacres+mac)
	recRespond={
    	'd0':unlock,
    	'd1':FailToUnlock}
	try:
    		print "Generating random number"
    		rn=os.urandom(32)
    		time.sleep(0.01)
    		print "The random number is:"
    		print rn.encode('hex')
    		mac=(hmac.new(intermediakey.decode('hex'),rn,hashlib.sha256)).hexdigest()
    		time.sleep(0.01)
    		print "The mac is:"
    		print mac
    		MAC_sent=mac.decode('hex')
    		sock.sendall(fixed_challenge+rn+MAC_sent)  
    		print "waiting for response......"
    		data = sock.recv(1460)
    		dataindex=data.encode('hex')
    		print 'the status code is',dataindex, 'from WINC1500'
    		func=recRespond[dataindex]
    		func()
	finally:
    		print "closing socket"
    		sock.close()


def authenticate2():
        global password

        # Generate fixed challenge to send to MCU (?)
        fixed_challenge = "bf9386b8-5a12-4cea-b105-672b417636ea"
        
        # Intermediate key


        # Generate random number and send to TCP server (Atmel)
        randNum = os.urandom(16) 
        clientSendSocket(PORT_STATUS,randNum)
        
        # Read response back from socket server and print


        # Compute host MAC value
        hostMAC = (hmac.new(fixed_challenge,randNum,hashlib.sha256)).hexdigest()
        print "Host MAC: ",  hostMAC
        clientSendSocket(PORT_STATUS, hostMAC)

        # Receive authentcation status from lock
        

######## GUI Setup ########



# Creating GUI labels and entry boxes
usernameLabel = SecureSmartLock.Label(lockRoot, text="Username")
newUsernameLabel = SecureSmartLock.Label(lockRoot, text="New Username")
usernameEntry = SecureSmartLock.Entry(lockRoot, exportselection='0')
newUsernameEntry = SecureSmartLock.Entry(lockRoot, exportselection='0')
passwordLabel = SecureSmartLock.Label(lockRoot, text="Password")
newPasswordLabel = SecureSmartLock.Label(lockRoot, text="New Password")
passwordEntry = SecureSmartLock.Entry(lockRoot, show="*", exportselection='0')
newPasswordEntry = SecureSmartLock.Entry(lockRoot, show="*", exportselection='0')
validForLabel = SecureSmartLock.Label(lockRoot, text="Valid For (minute)")
validForEntry = SecureSmartLock.Entry(lockRoot, exportselection='0')

# Organizing GUI labels and entry boxes
usernameLabel.pack(side="top",pady=3,padx=5)
usernameEntry.pack(side="top",pady=3,padx=5)
passwordLabel.pack(side="top",pady=3,padx=5)
passwordEntry.pack(side="top",pady=3,padx=5)

# Creating login button & interface
loginButton = SecureSmartLock.Button(lockRoot, text="Login", command=loginButtonClick)
loginButton.pack(side="top")

newUsernameLabel.pack(side="top",pady=3,padx=5)
newUsernameEntry.pack(side="top",pady=3,padx=5)
newPasswordLabel.pack(side="top",pady=3,padx=5)
newPasswordEntry.pack(side="top",pady=3,padx=5)
validForLabel.pack(side="top",pady=3,padx=5)
validForEntry.pack(side="top",pady=3,padx=5)


# Creating add User button & interface
addUserButton = SecureSmartLock.Button(lockRoot, text="Add User", command=addUser)
addUserButton.pack(side="top")

# Creating labels 
statusLabel  = SecureSmartLock.Label(lockRoot, text="Locked")
statusLabel.pack(side="left",pady=5)
unlockButton = SecureSmartLock.Button(lockRoot, text="Unlock", command=unlockButtonClick)
lockButton   = SecureSmartLock.Button(lockRoot, text="Lock", command=lockButtonClick)
unlockButton.pack(side="left",pady=5)
lockButton.pack(side="left")

# Creating GUI image that will represent lock status
statusImg = SecureSmartLock.Label(lockRoot, text="hello", image = imgLocked)
statusImg.pack(side="left",padx=15,pady=5)

# main()
if __name__=="__main__":

	# Run lockRoot GUI process periodically
	lockRoot.after(5000, checkCondition)
	lockRoot.mainloop()
        
       
