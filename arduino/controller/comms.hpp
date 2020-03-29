/*
 * Header file for comms.cpp
 */

 #ifndef COMMS_H
 #define COMMS_H

// This structure defines the messages exchanged with the desktop app.
typedef struct {
  // Patient 1
  double volume_p1;
  double pressure_p1;
  double flow_p1;
  double setpoint_p1;

  // Patient 2
  double volume_p2;
  double pressure_p2;
  double flow_p2;
  double setpoint_p2;
} message_t;


void comms_init(void);
void comms_send(message_t* msg);
void comms_receive(message_t* msg);

void debug_msg(const char* str);
void debug_int(const char* str, int x);
void debug_double(const char* str, double x);

#endif // COMMS_H
