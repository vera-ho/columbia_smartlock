package iotsmartlock.smartlockproject;


import android.util.Log;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.RelativeLayout;

import com.parse.Parse;
import com.parse.ParseObject;

import java.security.NoSuchAlgorithmException;
import java.security.MessageDigest;
import java.util.Random;

public class MainActivity extends AppCompatActivity {

    public final static String LOGIN_DATA = "com.iotsmartlock.smartlockproject.MESSAGE";
    private static final String TAG = "debug.info";
    public boolean authenticated;

    // Parse Keys
    public String appID = "29ZCCsazE4ZEDxQEv5yTE8qj4MJ5Og8oM4mVegdp";
    public String clientID = "uAqfuOPedeTjqmw6MkXC0tVcUxl1d6bNyz8pMMyF";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Enable Parse for "Android Lock"
        Parse.enableLocalDatastore(this);
        Parse.initialize(this, appID, clientID);

        setContentView(R.layout.activity_main);

        //ParseObject testObject = new ParseObject("TestObject");
        //testObject.put("foo", "bar");
        //testObject.saveInBackground();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        RelativeLayout main_view = (RelativeLayout) findViewById(R.id.main_view);

        Intent loggedInAct = new Intent(this, loggedInActivity.class);
        Intent alertsAct = new Intent(this, alertsActivity.class);
        Intent controlAct = new Intent(this, controlActivity.class);
        Intent newUserAct = new Intent(this, newuserActivity.class);

        switch(item.getItemId()) {

            case R.id.menu_login:
                // Already on login page
                Log.i(TAG, "Setting Clicked: Go to Login");
                return true;

            case R.id.menu_lockstatus:
                Log.i(TAG, "Setting Clicked: Go to Lock Options");
                startActivity(controlAct);
                return true;

            case R.id.menu_newuser:
                Log.i(TAG, "Setting Clicked: Go to New User");
                startActivity(newUserAct);
                return true;

            case R.id.menu_alerts:
                Log.i(TAG, "Setting Clicked: Go to Alerts");
                startActivity(alertsAct);
                return true;

            default:
                return super.onOptionsItemSelected(item);

        }

    }

    /** Called when the login button has been clicked
     *  Retrieves username and password information (save to a variable)
     *  Compares login information with Parse/Atmel (TBD)
     *  If login information is correct, start logged in activity
     *  Otherwise, display error message  **/
    public void authenticateUser(View view) {

        Intent intent = new Intent(this, loggedInActivity.class);

        EditText username = (EditText) findViewById(R.id.usernameEntry);
        EditText password = (EditText) findViewById(R.id.passwordEntry);
        TextView error = (TextView) findViewById(R.id.loginStatus);
        //TextView hashText = (TextView) findViewById(R.id.hashValue); for debug
        String usrMessage = username.getText().toString();
        String pwdMessage = password.getText().toString();
        Log.i(TAG, "Username: " + usrMessage);
        Log.i(TAG, "Password: " + pwdMessage);
        intent.putExtra(LOGIN_DATA, usrMessage);
        intent.putExtra(LOGIN_DATA, pwdMessage);

        // Generate a random number for authentication
        Random randNum = new Random();
        String random = new Integer(randNum.nextInt()).toString();
        Log.i(TAG, "Random Number: " + random);

        String hashed = getSHA256(pwdMessage, random);
        Log.i(TAG, "Hash Value: " + hashed);
        //hashText.setText(hashed);  // see hash for debug

        // Check if the user is authenticated
        if (authenticated){
            error.setText("Welcome Back!");
            startActivity(intent);
        } else {
            error.setText("Login Information Incorrect");
        }

    }

    /* Compute SHA-256 hash value for given strings
    *  Concatenates password and random number for unique values each time
    *  */
    public String getSHA256(String password, String randNum) {

        MessageDigest md = null;
        byte[] output;
        String s = randNum.concat(password);

        try {
            md = MessageDigest.getInstance("SHA-256");
        } catch (NoSuchAlgorithmException e){
            e.printStackTrace();
        }

        md.reset();
        md.update(s.getBytes());
        output = md.digest();

        StringBuffer hexString = new StringBuffer();
        for(int i=0; i<output.length;i++){
            hexString.append(Integer.toHexString(0xFF & output[i]));
        }

        return hexString.toString();
    }

}
