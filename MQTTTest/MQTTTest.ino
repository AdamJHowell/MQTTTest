#include <ESP8266WiFi.h>                  // Include the WiFi library.
#include <MQTT.h>                         // Include the MQTT library.
#include <Wire.h>                         // Include Wire library, required for I2C devices

const char* ssid     = "Red";             // The case-sensitive SSID of the WiFi network you want to connect to.
const char* password = "8012254722";      // The password of the WiFi network.
WiFiClient wifiClient;
MQTTClient mqttClient;
unsigned long lastMillis = 0;


void connect()
{
  Serial.print( "Checking wifi..." );
  while ( WiFi.status() != WL_CONNECTED )
  {
    Serial.print( "." );
    delay( 1000 );
  }

  Serial.print( "\nMQTT connecting..." );
  while ( !mqttClient.connect( "arduino", "try", "try" ) )
  {
    Serial.print( "." );
    delay( 1000 );
  }

  Serial.println( "\nMQTT connected!" );

  mqttClient.subscribe( "/hello" );
  // mqttClient.unsubscribe( "/hello" );
}


void messageReceived( String &topic, String &payload )
{
  Serial.println( "incoming: " + topic + " - " + payload );

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}


void setup()
{
  Serial.begin( 115200 );                 // Start the Serial communication to send messages to the computer.
  delay( 10 );
  Serial.println( '\n' );

  WiFi.begin( ssid, password );           // Connect to the network
  Serial.print( "WiFi connecting to " );
  Serial.println( ssid );

  int i = 0;
  /*
     WiFi.status() return values:
     0 : WL_IDLE_STATUS when WiFi is in process of changing between statuses
     1 : WL_NO_SSID_AVAIL in case configured SSID cannot be reached
     3 : WL_CONNECTED after successful connection is established
     4 : WL_CONNECT_FAILED if password is incorrect
     6 : WL_DISCONNECTED if module is not configured in station mode
  */
  while ( WiFi.status() != WL_CONNECTED ) // Wait for the WiFi to connect.
  {
    delay( 1000 );
    Serial.println( "Waiting for a connection..." );
    Serial.print( "WiFi status: " );
    Serial.println( WiFi.status() );
    Serial.print( ++i );
    Serial.println( " seconds" );
  }

  Serial.println( '\n' );
  Serial.println( "Connection established!" );
  Serial.print( "IP address:\t" );
  Serial.println( WiFi.localIP() );    // Send the IP address of the ESP8266 to the computer.

  pinMode( LED_BUILTIN, OUTPUT );      // Initialize digital pin LED_BUILTIN as an output.

  mqttClient.begin( "broker.shiftr.io", wifiClient );
  mqttClient.onMessage( messageReceived );

  connect();
}


void loop()
{
  mqttClient.loop();

  if ( !mqttClient.connected() )
  {
    connect();
  }

  // Publish a message roughly every second.
  if ( millis() - lastMillis > 1000 )
  {
    lastMillis = millis();
    mqttClient.publish( "/hello", "world" );
  }

  digitalWrite( LED_BUILTIN, HIGH );   // Turn the LED on.
  delay( 1000 );                       // Wait for one second.
  digitalWrite( LED_BUILTIN, LOW );    // Turn the LED off.
  delay( 1000 );                       // Wait for one second.
  Serial.print( "IP address:\t" );
  Serial.println( WiFi.localIP() );    // Send the IP address of the ESP8266 to the serial port.
}
