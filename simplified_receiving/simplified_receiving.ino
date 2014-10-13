
#include <XBee.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

void setup() {
  // start serial
  Serial.begin(9600);
  xbee.begin(Serial);
}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {
    xbee.readPacket();
    response = xbee.getResponse();
    
    if (response.isAvailable()) {
      // got something       
      if (response.getApiId() == ZB_RX_RESPONSE) {
        // got a zb rx packet
        // now fill our zb rx class
        response.getZBRxResponse(rx);
        for(int i = 0; i<rx.getDataLength(); i++){
         char x = (char) rx.getData(i);
         Serial.print(x);
        }
        
      }
    } else if (xbee.getResponse().isError()) {
      Serial.print("Error reading packet.  Error code: ");  
      Serial.println(xbee.getResponse().getErrorCode());
    }
}

