//
// Created by win7 on 2017/3/15 0015.
//

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <libusb.h>


#define  LOG_TAG    "TestDemo"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
//#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
//#define
#define printf LOGI
#define CONFIG_SIZE 8
#define INTERFACES 12
#define ENDPOINTS 24
int verbose = 0;
int num_interface = 0;
int num_endpoint = 0;

int endpoint_base = 0;
unsigned int ConfigureBuffer[CONFIG_SIZE]={0};
unsigned int InterfacesBuffer[INTERFACES][CONFIG_SIZE]={{0}};
unsigned int EndpointBuffer[ENDPOINTS][CONFIG_SIZE]={{0}};


static void print_endpoint_comp(const struct libusb_ss_endpoint_companion_descriptor *ep_comp)
{
	printf("      USB 3.0 Endpoint Companion:\n");
	printf("        bMaxBurst:        %d\n", ep_comp->bMaxBurst);
	printf("        bmAttributes:     0x%02x\n", ep_comp->bmAttributes);
	printf("        wBytesPerInterval: %d\n", ep_comp->wBytesPerInterval);
}


void setEndpointBuffers(int index,const struct libusb_endpoint_descriptor *endpoint){
    EndpointBuffer[index][0] = endpoint->bEndpointAddress;
    EndpointBuffer[index][1] = endpoint->bmAttributes;
    EndpointBuffer[index][2] = endpoint->wMaxPacketSize;
    EndpointBuffer[index][3] = endpoint->bInterval;
    EndpointBuffer[index][4] = endpoint->bSynchAddress;

}

static void print_endpoint(const struct libusb_endpoint_descriptor *endpoint)
{
	int i, ret;

	printf("      Endpoint:\n");
	printf("        bEndpointAddress: %02xh\n", endpoint->bEndpointAddress);
	printf("        bmAttributes:     %02xh\n", endpoint->bmAttributes);
	printf("        wMaxPacketSize:   %d\n", endpoint->wMaxPacketSize);
	printf("        bInterval:        %d\n", endpoint->bInterval);
	printf("        bRefresh:         %d\n", endpoint->bRefresh);
	printf("        bSynchAddress:    %d\n", endpoint->bSynchAddress);

	for (i = 0; i < endpoint->extra_length;) {
		if (LIBUSB_DT_SS_ENDPOINT_COMPANION == endpoint->extra[i + 1]) {
			struct libusb_ss_endpoint_companion_descriptor *ep_comp;

			ret = libusb_get_ss_endpoint_companion_descriptor(NULL, endpoint, &ep_comp);
			if (LIBUSB_SUCCESS != ret) {
				continue;
			}

			print_endpoint_comp(ep_comp);

			libusb_free_ss_endpoint_companion_descriptor(ep_comp);
		}

		i += endpoint->extra[i];
	}
}


void setInterfacesBuffers(int index,const struct libusb_interface_descriptor *interface){
    InterfacesBuffer[index][0] = interface->bInterfaceNumber;
    InterfacesBuffer[index][1] = interface->bAlternateSetting;
    InterfacesBuffer[index][2] = interface->bNumEndpoints;
    InterfacesBuffer[index][3] = interface->bInterfaceClass;
    InterfacesBuffer[index][4] = interface->bInterfaceSubClass;
    InterfacesBuffer[index][5] = interface->bInterfaceProtocol;
    InterfacesBuffer[index][6] = interface->iInterface;
}
static void print_altsetting(const struct libusb_interface_descriptor *interface)
{
	int i;

	printf("    Interface:\n");
	printf("      bInterfaceNumber:   %d\n", interface->bInterfaceNumber);
	printf("      bAlternateSetting:  %d\n", interface->bAlternateSetting);
	printf("      bNumEndpoints:      %d\n", interface->bNumEndpoints);
	printf("      bInterfaceClass:    %d\n", interface->bInterfaceClass);
	printf("      bInterfaceSubClass: %d\n", interface->bInterfaceSubClass);
	printf("      bInterfaceProtocol: %d\n", interface->bInterfaceProtocol);
	printf("      iInterface:         %d\n", interface->iInterface);

    num_endpoint += interface->bNumEndpoints;
	for (i = 0; i < interface->bNumEndpoints; i++){
	setEndpointBuffers(endpoint_base,&interface->endpoint[i]);
	endpoint_base += (i+1);
		print_endpoint(&interface->endpoint[i]);
		}
}

static void print_2_0_ext_cap(struct libusb_usb_2_0_extension_descriptor *usb_2_0_ext_cap)
{
	printf("    USB 2.0 Extension Capabilities:\n");
	printf("      bDevCapabilityType: %d\n", usb_2_0_ext_cap->bDevCapabilityType);
	printf("      bmAttributes:       0x%x\n", usb_2_0_ext_cap->bmAttributes);
}

static void print_ss_usb_cap(struct libusb_ss_usb_device_capability_descriptor *ss_usb_cap)
{
	printf("    USB 3.0 Capabilities:\n");
	printf("      bDevCapabilityType: %d\n", ss_usb_cap->bDevCapabilityType);
	printf("      bmAttributes:       0x%x\n", ss_usb_cap->bmAttributes);
	printf("      wSpeedSupported:    0x%x\n", ss_usb_cap->wSpeedSupported);
	printf("      bFunctionalitySupport: %d\n", ss_usb_cap->bFunctionalitySupport);
	printf("      bU1devExitLat:      %d\n", ss_usb_cap->bU1DevExitLat);
	printf("      bU2devExitLat:      %d\n", ss_usb_cap->bU2DevExitLat);
}

static void print_bos(libusb_device_handle *handle)
{
	struct libusb_bos_descriptor *bos;
	int ret;

	ret = libusb_get_bos_descriptor(handle, &bos);
	if (0 > ret) {
		return;
	}

	printf("  Binary Object Store (BOS):\n");
	printf("    wTotalLength:       %d\n", bos->wTotalLength);
	printf("    bNumDeviceCaps:     %d\n", bos->bNumDeviceCaps);

	if(bos->dev_capability[0]->bDevCapabilityType == LIBUSB_BT_USB_2_0_EXTENSION) {

		struct libusb_usb_2_0_extension_descriptor *usb_2_0_extension;
	        ret =  libusb_get_usb_2_0_extension_descriptor(NULL, bos->dev_capability[0],&usb_2_0_extension);
	        if (0 > ret) {
		        return;
	        }

                print_2_0_ext_cap(usb_2_0_extension);
                libusb_free_usb_2_0_extension_descriptor(usb_2_0_extension);
        }

	if(bos->dev_capability[0]->bDevCapabilityType == LIBUSB_BT_SS_USB_DEVICE_CAPABILITY) {

	        struct libusb_ss_usb_device_capability_descriptor *dev_cap;
		ret = libusb_get_ss_usb_device_capability_descriptor(NULL, bos->dev_capability[0],&dev_cap);
	        if (0 > ret) {
		        return;
	        }

	        print_ss_usb_cap(dev_cap);
	        libusb_free_ss_usb_device_capability_descriptor(dev_cap);
        }

	libusb_free_bos_descriptor(bos);
}

static void print_interface(const struct libusb_interface *interface)
{
	int i;
    printf("number of interface = %d.\n",interface->num_altsetting);
    num_interface += interface->num_altsetting;
	for (i = 0; i < interface->num_altsetting; i++){
	    if(interface->num_altsetting == 1)
	        setInterfacesBuffers(0,&interface->altsetting[i]);
	    else
	        setInterfacesBuffers(i+1,&interface->altsetting[i]);
		print_altsetting(&interface->altsetting[i]);
		}
}

void setBuffers(struct libusb_config_descriptor *config){
    ConfigureBuffer[0] = config->wTotalLength;
    ConfigureBuffer[1] = config->bNumInterfaces;
    ConfigureBuffer[2] = config->bConfigurationValue;
    ConfigureBuffer[3] = config->iConfiguration;
    ConfigureBuffer[4] = config->bmAttributes;
    ConfigureBuffer[5] = config->MaxPower;
  //  ConfigureBuffer[6] = config->wTotalLength;
}

static void print_configuration(struct libusb_config_descriptor *config)
{
	int i;

	printf("  Configuration:\n");
	printf("    wTotalLength:         %d\n", config->wTotalLength);
	printf("    bNumInterfaces:       %d\n", config->bNumInterfaces);
	printf("    bConfigurationValue:  %d\n", config->bConfigurationValue);
	printf("    iConfiguration:       %d\n", config->iConfiguration);
	printf("    bmAttributes:         %02xh\n", config->bmAttributes);
	printf("    MaxPower:             %d\n", config->MaxPower);

setBuffers(config);

	for (i = 0; i < config->bNumInterfaces; i++)
		print_interface(&config->interface[i]);
}

static int print_device(libusb_device *dev, int level)
{
	struct libusb_device_descriptor desc;
	libusb_device_handle *handle = NULL;
	char description[256];
	char string[256];
	int ret, i;

	ret = libusb_get_device_descriptor(dev, &desc);
	if (ret < 0) {
		fprintf(stderr, "failed to get device descriptor");
		return -1;
	}

	ret = libusb_open(dev, &handle);
	if (LIBUSB_SUCCESS == ret) {
		if (desc.iManufacturer) {
			ret = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, string, sizeof(string));
			if (ret > 0)
				snprintf(description, sizeof(description), "%s - ", string);
			else
				snprintf(description, sizeof(description), "%04X - ",
				desc.idVendor);
		}
		else
			snprintf(description, sizeof(description), "%04X - ",
			desc.idVendor);

		if (desc.iProduct) {
			ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, string, sizeof(string));
			if (ret > 0)
				snprintf(description + strlen(description), sizeof(description) -
				strlen(description), "%s", string);
			else
				snprintf(description + strlen(description), sizeof(description) -
				strlen(description), "%04X", desc.idProduct);
		}
		else
			snprintf(description + strlen(description), sizeof(description) -
			strlen(description), "%04X", desc.idProduct);
	}
	else {
		snprintf(description, sizeof(description), "%04X - %04X",
			desc.idVendor, desc.idProduct);
	}

	printf("%.*sDev (bus %d, device %d): %s\n", level * 2, "                    ",
		libusb_get_bus_number(dev), libusb_get_device_address(dev), description);

	if (handle && verbose) {
		if (desc.iSerialNumber) {
			ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, string, sizeof(string));
			if (ret > 0)
				printf("%.*s  - Serial Number: %s\n", level * 2,
				"                    ", string);
		}
	}

	if (verbose) {
		for (i = 0; i < desc.bNumConfigurations; i++) {
			struct libusb_config_descriptor *config;
			ret = libusb_get_config_descriptor(dev, i, &config);
			if (LIBUSB_SUCCESS != ret) {
				printf("  Couldn't retrieve descriptors\n");
				continue;
			}

			print_configuration(config);

			libusb_free_config_descriptor(config);
		}


		if (handle && desc.bcdUSB >= 0x0201) {
			print_bos(handle);
		}
	}

	if (handle)
		libusb_close(handle);

	return 0;
}




static void print_devs(libusb_device **devs)
{
	libusb_device *dev;
	int i = 0, j = 0;
	uint8_t path[8];

	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			fprintf(stderr, "failed to get device descriptor");
			return;
		}

		LOGI("%04x:%04x (bus %d, device %d)",
			desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));

		r = libusb_get_port_numbers(dev, path, sizeof(path));
		if (r > 0) {
			LOGI(" path: %d", path[0]);
			for (j = 1; j < r; j++)
				LOGI(".%d", path[j]);
		}
		LOGI("\n");
	}
}

int usb_find_orbbec_device(
    libusb_device *dev,
    int vid, int pid, const char *sn) {
  int found_dev;
int ret;
  found_dev = 0;

   struct libusb_device_descriptor desc;
   ret = libusb_get_device_descriptor(dev, &desc);
    if (ret < 0) {
    	fprintf(stderr, "failed to get device descriptor");
    	return -1;
    }

    if ((!vid || desc.idVendor == vid)
        && (!pid || desc.idProduct == pid)
       )
      found_dev = 1;

  if (found_dev)
    libusb_ref_device(dev);


  if (found_dev) {
    return 1;
    }
  else{
    return 0;
    }
}

int usbinit()
{
    libusb_device **devs;
    int r,cnt;
    r = libusb_init(NULL);
if (r < 0)
		return r;

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0)
		return  cnt;

	print_devs(devs);
	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);

 return 0;
}



int testlibusb( )
{
	libusb_device **devs;
	ssize_t cnt;
	int r, i;

	//if (argc > 1 && !strcmp(argv[1], "-v"))
		verbose = 1;

	r = libusb_init(NULL);
	if (r < 0)
		return r;

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0)
		return (int)cnt;

	for (i = 0; devs[i]; ++i) {
	    if(usb_find_orbbec_device(devs[i],11205,1281, NULL) == 1)
		    print_device(devs[i], 0);
	}

	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);
	return 0;
}



jint
Java_wmy_jni_com_usbdemo_NativeMethod_testUSB(JNIEnv* env,jobject thiz,jint *it,jint *ep){
//usbinit();
testlibusb( );
LOGI("222222222");
it = num_interface;
ep = num_endpoint;
LOGI("IT = %d.ep = %d .\n",it,ep);
  return 0;

}

jint
Java_wmy_jni_com_usbdemo_NativeMethod_getNumInterfaces(JNIEnv* env,jobject thiz){
    return num_interface;
}
jint
Java_wmy_jni_com_usbdemo_NativeMethod_getNumEndpoints(JNIEnv* env,jobject thiz){
    return num_endpoint;
}

jintArray
Java_wmy_jni_com_usbdemo_NativeMethod_getConfigBuffers(JNIEnv* env,jobject thiz){

jintArray  array = (*env)->NewIntArray(env,CONFIG_SIZE);
(*env)->SetIntArrayRegion(env,array,0,CONFIG_SIZE,ConfigureBuffer);

return array;
}

jintArray
Java_wmy_jni_com_usbdemo_NativeMethod_getInterfaceBuffers(JNIEnv* env,jobject thiz,jint index){

jintArray  array = (*env)->NewIntArray(env,CONFIG_SIZE);
(*env)->SetIntArrayRegion(env,array,0,CONFIG_SIZE,InterfacesBuffer[index]);

return array;

}

jintArray
Java_wmy_jni_com_usbdemo_NativeMethod_getEndpointBuffers(JNIEnv* env,jobject thiz,jint index){

jintArray  array = (*env)->NewIntArray(env,CONFIG_SIZE);
(*env)->SetIntArrayRegion(env,array,0,CONFIG_SIZE,EndpointBuffer[index]);

return array;

}