import processing.serial.*;
import java.io.FileWriter;
import java.io.*;

Serial port;
String data;
boolean state = false;
char label = '3';

void setup() {
  size(200, 400);
  port = new Serial(this, "/dev/ttyUSB0", 115200);
  port.bufferUntil('\n');
}

void draw() {
  noStroke();
  fill(0, 0, 0);
  rect(0, 0, 200, 100);
  fill(255, 0, 0);
  rect(0, 100, 200, 100);
  fill(0, 255, 0);
  rect(0, 200, 200, 300);
  fill(0, 0, 255);
  rect(0, 300, 200, 400);
}

void serialEvent (Serial port) {
  data = port.readStringUntil('\n');
  //println(data);
  if(label != '3') {
    saveData();
  }
}

void mouseClicked() {
  if(mouseY < 100) {
    label = '3';
    println("done...");
  }
  else if(mouseY < 200) {
    label = '0';
    println("taking data...0");
  }
  else if(mouseY < 300) {
    label = '1';
    println("taking data...1");
  }
  else {
    label = '2';
    println("taking data...2");
  }
}

void saveData() {
  try {
    File file =new File(sketchPath() + "/bla/train_data.csv");
    if (!file.exists()) {
      file.createNewFile();
    }
    FileWriter fw = new FileWriter(file, true);
    BufferedWriter bw = new BufferedWriter(fw);
    PrintWriter pw = new PrintWriter(bw);
    pw.write(data);
    pw.close();
  }
  catch(IOException ioe) {
    System.out.println("Exception ");
    ioe.printStackTrace();
  }
  
  try {
    File file =new File(sketchPath() + "/bla/train_labels.csv");
    if (!file.exists()) {
      file.createNewFile();
    }
    FileWriter fw = new FileWriter(file, true);
    BufferedWriter bw = new BufferedWriter(fw);
    PrintWriter pw = new PrintWriter(bw);
    pw.write(label);
    pw.write('\n');
    pw.close();
  }
  catch(IOException ioe) {
    System.out.println("Exception ");
    ioe.printStackTrace();
  }
}
