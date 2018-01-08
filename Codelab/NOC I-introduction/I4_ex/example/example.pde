
import java.util.Random;

Random generator;
void setup() {
    size(400, 300);
    generator = new Random();
}

void draw() {
  background(255);
    
  float h = (float) generator.nextGaussian();
  
  h = h* 10;
  h = h + 100;

  fill(0);
  ellipse(width/2,height/2,h,h);
 
}