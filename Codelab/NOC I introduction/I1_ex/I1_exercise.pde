class Walker { // data
  float x, y;

  Walker() { // constructor
  x = width/2;
  y = height/2;
  }

  void render() { // Objects have functions
  stroke(0);
  point(x,y);
  }

   void step() { // more efficient way of setting up randomness, 9 options
     float stepx = random(-0.5, 2);
     float stepy = random(-0.5, 2);
     x += stepx;
     y += stepy;
   }
}

Walker w;
 void setup() {
  size(640,360); // skywalker is alive!
  w = new Walker();
  background(255);
  }

  void draw() { // make it do stuff
    w.step();
    w.render();
  }
