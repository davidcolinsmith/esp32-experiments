// GATE output pins
#define GATEout_0 25
#define GATEout_1 26
#define GATEout_2 4
#define GATEout_3 27

void GATEout(int channel, bool value) {
  switch (channel) {
    case 0:
       digitalWrite(GATEout_0,value); //
       break;
    case 1:
       digitalWrite(GATEout_1,value); //
       break;
    case 2:
       digitalWrite(GATEout_2,value); //
       break;
    case 3:
       digitalWrite(GATEout_3,value); //
       break;
  }
}

