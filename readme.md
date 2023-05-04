# Billing System using RFID & ESP 8266

### Run Local
- Folder `./arduino_codes` for arduino codes.
- Install requirements from `requirements.txt`.
- Fill `MONGODB_API` variable in utilities/database.py.
- Run `python3 main.py`.
<hr>
Open browser to <code>http://localhost:6789/docs</code>.
<br>
Use this api to and create your own Web/Mobile application,<br> 
Note ⚠️: The endpoints <code>/user/regiser</code> and <code>/product/new</code> does not genereate <code>id</code> param ATM.  (currently it doesn't create unique <code>id</code>).
<hr>

### API Reference: https://apibackend-1-u9070050.deta.app/docs
Just for reference, you cannot execute queries
<hr>

### In (./arduino_codes):
- Replace `serverUrl` with your value.
- (if https) Replace `fingerprint` with SHA1 fingerprint of api
<hr>

## Flutter Application
soon...
