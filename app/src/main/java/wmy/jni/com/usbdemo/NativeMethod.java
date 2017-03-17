package wmy.jni.com.usbdemo;

/**
 * Created by win7 on 2017/3/15 0015.
 */

public class NativeMethod {

    static {
        System.loadLibrary("usb_interface");
    }

    public native int testUSB();
    public native int[] getConfigBuffers();
    public native int[] getInterfaceBuffers(int index);
    public native int[] getEndpointBuffers(int index);
    public native int getNumEndpoints();
    public native int getNumInterfaces();

}
