import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class I1_exercise extends PApplet {

class Walker { // data
  float x, y;

  Walker() { // constructor
  x = width/2;
  y = height/2;
  }

  public void render() { // Objects have functions
  stroke(0);
  point(x,y);
  }

   public void step() { // more efficient way of setting up randomness, 9 options
     float stepx = random(-0.5f, 2);
     float stepy = random(-0.5f, 2);
     x += stepx;
     y += stepy;
   }
}

Walker w;
 public void setup() {
   // skywalker is alive!
  w = new Walker();
  background(255);
  }

  public void draw() { // make it do stuff
    w.step();
    w.render();
  }
  public void settings() {  size(640,360); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "I1_exercise" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
