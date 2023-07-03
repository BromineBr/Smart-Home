

#ifndef DHT11_H
#define DHT11_H



#ifndef _ADAFRUIT_SENSOR_H
#define _ADAFRUIT_SENSOR_H

#ifndef ARDUINO
#include <stdint.h>
#elif ARDUINO >= 100
#include "Arduino.h"
#include "Print.h"
#else
#include "WProgram.h"
#endif

/* Constants */
#define SENSORS_GRAVITY_EARTH (9.80665F) /**< Earth's gravity in m/s^2 */
#define SENSORS_GRAVITY_MOON (1.6F)      /**< The moon's gravity in m/s^2 */
#define SENSORS_GRAVITY_SUN (275.0F)     /**< The sun's gravity in m/s^2 */
#define SENSORS_GRAVITY_STANDARD (SENSORS_GRAVITY_EARTH)
#define SENSORS_MAGFIELD_EARTH_MAX                                             \
  (60.0F) /**< Maximum magnetic field on Earth's surface */
#define SENSORS_MAGFIELD_EARTH_MIN                                             \
  (30.0F) /**< Minimum magnetic field on Earth's surface */
#define SENSORS_PRESSURE_SEALEVELHPA                                           \
  (1013.25F) /**< Average sea level pressure is 1013.25 hPa */
#define SENSORS_DPS_TO_RADS                                                    \
  (0.017453293F) /**< Degrees/s to rad/s multiplier                            \
                  */
#define SENSORS_RADS_TO_DPS                                                    \
  (57.29577793F) /**< Rad/s to degrees/s  multiplier */
#define SENSORS_GAUSS_TO_MICROTESLA                                            \
  (100) /**< Gauss to micro-Tesla multiplier */

/** Sensor types */
typedef enum {
  SENSOR_TYPE_ACCELEROMETER = (1), /**< Gravity + linear acceleration */
  SENSOR_TYPE_MAGNETIC_FIELD = (2),
  SENSOR_TYPE_ORIENTATION = (3),
  SENSOR_TYPE_GYROSCOPE = (4),
  SENSOR_TYPE_LIGHT = (5),
  SENSOR_TYPE_PRESSURE = (6),
  SENSOR_TYPE_PROXIMITY = (8),
  SENSOR_TYPE_GRAVITY = (9),
  SENSOR_TYPE_LINEAR_ACCELERATION =
      (10), /**< Acceleration not including gravity */
  SENSOR_TYPE_ROTATION_VECTOR = (11),
  SENSOR_TYPE_RELATIVE_HUMIDITY = (12),
  SENSOR_TYPE_AMBIENT_TEMPERATURE = (13),
  SENSOR_TYPE_OBJECT_TEMPERATURE = (14),
  SENSOR_TYPE_VOLTAGE = (15),
  SENSOR_TYPE_CURRENT = (16),
  SENSOR_TYPE_COLOR = (17)
} sensors_type_t;

/** struct sensors_vec_s is used to return a vector in a common format. */
typedef struct {
  union {
    float v[3]; ///< 3D vector elements
    struct {
      float x; ///< X component of vector
      float y; ///< Y component of vector
      float z; ///< Z component of vector
    };         ///< Struct for holding XYZ component
    /* Orientation sensors */
    struct {
      float roll; /**< Rotation around the longitudinal axis (the plane body, 'X
                     axis'). Roll is positive and increasing when moving
                     downward. -90 degrees <= roll <= 90 degrees */
      float pitch;   /**< Rotation around the lateral axis (the wing span, 'Y
                        axis'). Pitch is positive and increasing when moving
                        upwards. -180 degrees <= pitch <= 180 degrees) */
      float heading; /**< Angle between the longitudinal axis (the plane body)
                        and magnetic north, measured clockwise when viewing from
                        the top of the device. 0-359 degrees */
    };               ///< Struct for holding roll/pitch/heading
  };                 ///< Union that can hold 3D vector array, XYZ components or
                     ///< roll/pitch/heading
  int8_t status;     ///< Status byte
  uint8_t reserved[3]; ///< Reserved
} sensors_vec_t;

/** struct sensors_color_s is used to return color data in a common format. */
typedef struct {
  union {
    float c[3]; ///< Raw 3-element data
    /* RGB color space */
    struct {
      float r;   /**< Red component */
      float g;   /**< Green component */
      float b;   /**< Blue component */
    };           ///< RGB data in floating point notation
  };             ///< Union of various ways to describe RGB colorspace
  uint32_t rgba; /**< 24-bit RGBA value */
} sensors_color_t;

/* Sensor event (36 bytes) */
/** struct sensor_event_s is used to provide a single sensor event in a common
 * format. */
typedef struct {
  int32_t version;   /**< must be sizeof(struct sensors_event_t) */
  int32_t sensor_id; /**< unique sensor identifier */
  int32_t type;      /**< sensor type */
  int32_t reserved0; /**< reserved */
  int32_t timestamp; /**< time is in milliseconds */
  union {
    float data[4];              ///< Raw data
    sensors_vec_t acceleration; /**< acceleration values are in meter per second
                                   per second (m/s^2) */
    sensors_vec_t
        magnetic; /**< magnetic vector values are in micro-Tesla (uT) */
    sensors_vec_t orientation; /**< orientation values are in degrees */
    sensors_vec_t gyro;        /**< gyroscope values are in rad/s */
    float temperature; /**< temperature is in degrees centigrade (Celsius) */
    float distance;    /**< distance in centimeters */
    float light;       /**< light in SI lux units */
    float pressure;    /**< pressure in hectopascal (hPa) */
    float relative_humidity; /**< relative humidity in percent */
    float current;           /**< current in milliamps (mA) */
    float voltage;           /**< voltage in volts (V) */
    sensors_color_t color;   /**< color in RGB component values */
  };                         ///< Union for the wide ranges of data we can carry
} sensors_event_t;

/* Sensor details (40 bytes) */
/** struct sensor_s is used to describe basic information about a specific
 * sensor. */
typedef struct {
  char name[12];     /**< sensor name */
  int32_t version;   /**< version of the hardware + driver */
  int32_t sensor_id; /**< unique sensor identifier */
  int32_t type;      /**< this sensor's type (ex. SENSOR_TYPE_LIGHT) */
  float max_value;   /**< maximum value of this sensor's value in SI units */
  float min_value;   /**< minimum value of this sensor's value in SI units */
  float resolution; /**< smallest difference between two values reported by this
                       sensor */
  int32_t min_delay; /**< min delay in microseconds between events. zero = not a
                        constant rate */
} sensor_t;


class Adafruit_Sensor {
public:
  // Constructor(s)
  Adafruit_Sensor() {}
  virtual ~Adafruit_Sensor() {}

  // These must be defined by the subclass

  /*! @brief Whether we should automatically change the range (if possible) for
     higher precision
      @param enabled True if we will try to autorange */
  virtual void enableAutoRange(bool enabled) {
    (void)enabled; /* suppress unused warning */
  };

  /*! @brief Get the latest sensor event
      @returns True if able to fetch an event */
  virtual bool getEvent(sensors_event_t *) = 0;
  /*! @brief Get info about the sensor itself */
  virtual void getSensor(sensor_t *) = 0;

  void printSensorDetails(void);

private:
  bool _autoRange;
};

#endif
#ifndef DHT_H
#define DHT_H

#include "Arduino.h"

/* Uncomment to enable printing out nice debug messages. */
//#define DHT_DEBUG

#define DEBUG_PRINTER                                                          \
  Serial /**< Define where debug output will be printed.                       \
          */

/* Setup debug printing macros. */
#ifdef DHT_DEBUG
#define DEBUG_PRINT(...)                                                       \
  { DEBUG_PRINTER.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...)                                                     \
  { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define DEBUG_PRINT(...)                                                       \
  {} /**< Debug Print Placeholder if Debug is disabled */
#define DEBUG_PRINTLN(...)                                                     \
  {} /**< Debug Print Line Placeholder if Debug is disabled */
#endif

/* Define types of sensors. */
#define DHT11 11  /**< DHT TYPE 11 */
#define DHT12 12  /**< DHY TYPE 12 */
#define DHT22 22  /**< DHT TYPE 22 */
#define DHT21 21  /**< DHT TYPE 21 */
#define AM2301 21 /**< AM2301 */

#if (TARGET_NAME == ARDUINO_NANO33BLE)
#ifndef microsecondsToClockCycles
/*!
 * As of 7 Sep 2020 the Arduino Nano 33 BLE boards do not have
 * microsecondsToClockCycles defined.
 */
#define microsecondsToClockCycles(a) ((a) * (SystemCoreClock / 1000000L))
#endif
#endif

/*!
 *  @brief  Class that stores state and functions for DHT
 */
class DHT {
public:
  DHT(uint8_t pin, uint8_t type, uint8_t count = 6);
  void begin(uint8_t usec = 55);
  float readTemperature(bool S = false, bool force = false);
  float convertCtoF(float);
  float convertFtoC(float);
  float computeHeatIndex(bool isFahrenheit = true);
  float computeHeatIndex(float temperature, float percentHumidity,
                         bool isFahrenheit = true);
  float readHumidity(bool force = false);
  bool read(bool force = false);

private:
  uint8_t data[5];
  uint8_t _pin, _type;
#ifdef __AVR
  // Use direct GPIO access on an 8-bit AVR so keep track of the port and
  // bitmask for the digital pin connected to the DHT.  Other platforms will use
  // digitalRead.
  uint8_t _bit, _port;
#endif
  uint32_t _lastreadtime, _maxcycles;
  bool _lastresult;
  uint8_t pullTime; // Time (in usec) to pull up data line before reading

  uint32_t expectPulse(bool level);
};

/*!
 *  @brief  Class that defines Interrupt Lock Avaiability
 */
class InterruptLock {
public:
  InterruptLock() {
#if !defined(ARDUINO_ARCH_NRF52)
    noInterrupts();
#endif
  }
  ~InterruptLock() {
#if !defined(ARDUINO_ARCH_NRF52)
    interrupts();
#endif
  }
};

#endif

#define DHT_SENSOR_VERSION 1 /**< Sensor Version */

/*!
 *  @brief  Class that stores state and functions for interacting with
 * DHT_Unified.
 */
class DHT_Unified {
public:
  DHT_Unified(uint8_t pin, uint8_t type, uint8_t count = 6,
              int32_t tempSensorId = -1, int32_t humiditySensorId = -1);
  void begin();

  /*!
   *  @brief  Class that stores state and functions about Temperature
   */
  class Temperature : public Adafruit_Sensor {
  public:
    Temperature(DHT_Unified *parent, int32_t id);
    bool getEvent(sensors_event_t *event);
    void getSensor(sensor_t *sensor);

  private:
    DHT_Unified *_parent;
    int32_t _id;
  };

  /*!
   *  @brief  Class that stores state and functions about Humidity
   */
  class Humidity : public Adafruit_Sensor {
  public:
    Humidity(DHT_Unified *parent, int32_t id);
    bool getEvent(sensors_event_t *event);
    void getSensor(sensor_t *sensor);

  private:
    DHT_Unified *_parent;
    int32_t _id;
  };

  /*!
   *  @brief  Returns temperature stored in _temp
   *  @return Temperature value
   */
  Temperature temperature() { return _temp; }

  /*!
   *  @brief  Returns humidity stored in _humidity
   *  @return Humidity value
   */
  Humidity humidity() { return _humidity; }

private:
  DHT _dht;
  uint8_t _type;
  Temperature _temp;
  Humidity _humidity;

  void setName(sensor_t *sensor);
  void setMinDelay(sensor_t *sensor);
};

#endif
