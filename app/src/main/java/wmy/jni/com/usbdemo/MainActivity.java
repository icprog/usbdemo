package wmy.jni.com.usbdemo;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.ListViewCompat;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {



    private ListView list;
    private ListView list1;
    private TextView Des;
    int endpoint_base = 0;
    List<String> data ;
    NativeMethod nm;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        data = new ArrayList<String>();
        nm = new NativeMethod();
        int []it = new int[5];
        int []ep = new int[5];
        int value = nm.testUSB();
        /*
        Log.d("","return value = "+ value+"it = "+nm.getNumInterfaces()+"ep = "+nm.getNumEndpoints());
        Log.d("","0."+nm.getConfigBuffers()[0]);
        Log.d("","1."+nm.getConfigBuffers()[1]);
        Log.d("","2."+nm.getConfigBuffers()[2]);
        Log.d("","3."+nm.getConfigBuffers()[3]);
        Log.d("","4."+Integer.toHexString(nm.getConfigBuffers()[4])+"h");
        Log.d("","5."+nm.getConfigBuffers()[5]);
*/
        findview();

        Des = new TextView(this);
        Des.setText("ssssss");

        list.setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_expandable_list_item_1,getTitleItem()));
        list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                if(i == 7)
                {
                    Toast.makeText(MainActivity.this,"click 1",Toast.LENGTH_LONG).show();
                }
                if(i == 8)
                {
                    Toast.makeText(MainActivity.this,"click 2",Toast.LENGTH_LONG).show();

                }

            }
        });

        list1.setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_expandable_list_item_1,getData()));

          //  list1.setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_expandable_list_item_1,getInterfaceData(i)));

    }
    private List<String> getTitleItem(){

        List<String> data1 = new ArrayList<String>();
        data1.add("Configuretion");
        data1.add("wTotalLength");
        data1.add("bNumInterfaces");
        data1.add("bConfigurationValue");
        data1.add("iConfiguration");
        data1.add("bmAttributes");
        data1.add("MaxPower");
        data1.add("numOfInterface");
        data1.add("numOfEndpoint");
        return data1;
    }
    private List<String> getData(){


        data.add("Value");
        data.add(""+nm.getConfigBuffers()[0]);
        data.add(""+nm.getConfigBuffers()[1]);
        data.add(""+nm.getConfigBuffers()[2]);
        data.add(""+nm.getConfigBuffers()[3]);
        data.add(""+Integer.toHexString(nm.getConfigBuffers()[4])+"h");
        data.add(""+nm.getConfigBuffers()[5]);
        data.add(""+nm.getNumInterfaces());
        data.add(""+nm.getNumEndpoints());

        for(int i = 0; i < nm.getNumInterfaces();i++)
            getInterfaceData(i);
        return data;
    }

    private List<String> getInterfaceData(int index){

       // List<String> data = new ArrayList<String>();
        data.add("Interface:");
        data.add(""+nm.getInterfaceBuffers(index)[0]);
        data.add(""+nm.getInterfaceBuffers(index)[1]);
        data.add(""+nm.getInterfaceBuffers(index)[2]);
        data.add(""+nm.getInterfaceBuffers(index)[3]);
        data.add(""+nm.getInterfaceBuffers(index)[4]);
        data.add(""+nm.getInterfaceBuffers(index)[5]);
        data.add(""+nm.getInterfaceBuffers(index)[6]);
        if(nm.getInterfaceBuffers(index)[2] != 0){
            int i;
            int InterfaceEndpoint = nm.getInterfaceBuffers(index)[2];
            for(i  = endpoint_base; i < nm.getNumEndpoints() ; i++) {
                getEndpointData(i);
                InterfaceEndpoint--;
                if(InterfaceEndpoint == 0)
                    break;
            }
            endpoint_base += nm.getInterfaceBuffers(index)[2];
        }

       // data.add(""+nm.getInterfacesBuffers()[7]);
        return data;
    }
    private List<String> getEndpointData(int index){

       // List<String> data = new ArrayList<String>();
        data.add("Endpoint:");
        data.add(""+Integer.toHexString(nm.getEndpointBuffers(index)[0])+"h");
        data.add(""+Integer.toHexString(nm.getEndpointBuffers(index)[1])+"h");
        data.add(""+nm.getEndpointBuffers(index)[2]);
        data.add(""+nm.getEndpointBuffers(index)[3]);
        data.add(""+nm.getEndpointBuffers(index)[4]);
        data.add(""+nm.getEndpointBuffers(index)[5]);
      //  data.add(""+nm.getEndpointBuffers());
      //  data.add(""+nm.getEndpointBuffers());

        return data;
    }

    private void findview() {
       list = (ListView)findViewById(R.id.listview);
        list1 = (ListView)findViewById(R.id.listview2);
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
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
