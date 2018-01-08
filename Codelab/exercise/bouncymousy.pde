float boxPositionFloat;
float speed = 0.5;
float acceleration = 1.5;
PImage mousy;
PImage mousy2;
PImage temporaryMouse;
PImage cat;

void setup (){
  size(600,500);
  mousy = loadImage("mousy.png");  
  mousy2 = loadImage("mousy2.png");
  cat = loadImage("catty.jpg");
  cat.resize(width, height);
  background(cat);
}

void draw() {
   background(cat);
   rect(mouseX,boxPositionFloat,100,100,0);
   fill(random(255),random(255),random(255),0);
   stroke(0,0,0,0);
    boxPositionFloat = boxPositionFloat + speed;
    speed = speed + acceleration;
    
    image(mousy,mouseX, boxPositionFloat);
    //if (speed>0) {
    //  image(mousy,150, boxPositionFloat);
    //} else {
    //  image(mousy2,150, boxPositionFloat);
    //}
    
if (boxPositionFloat>500-100) {
    boxPositionFloat = 500-100;  
    //image(mousy2, mouseY);
    speed = speed * -0.9;
   
    
   
    temporaryMouse=mousy;
    mousy = mousy2;
    mousy2 = temporaryMouse;
    
    //if (speed>0) {
    //mousy = mousy2; 
    //}
    //else {
    //mousy=temporaryMouse;
    //}
   
  }
}