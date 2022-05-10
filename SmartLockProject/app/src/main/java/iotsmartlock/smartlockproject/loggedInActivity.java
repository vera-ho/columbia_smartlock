package iotsmartlock.smartlockproject;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.RelativeLayout;

public class loggedInActivity extends AppCompatActivity {

    private static final String TAG = "debug.info";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_logged_in);


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

        Intent loginAct = new Intent(this, MainActivity.class);
        Intent alertsAct = new Intent(this, alertsActivity.class);
        Intent controlAct = new Intent(this, controlActivity.class);
        Intent newUserAct = new Intent(this, newuserActivity.class);

        boolean timedout = false;

        switch(item.getItemId()) {

            case R.id.menu_login:
                if(timedout){
                    // Already in logged in activity
                    Log.i(TAG, "Setting Clicked: Stay on Logged in page");
                } else {
                    Log.i(TAG, "Setting Clicked: Go to Log In");
                    startActivity(loginAct);
                }
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
}
