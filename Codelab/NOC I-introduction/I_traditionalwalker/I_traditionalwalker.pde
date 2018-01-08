class Walker { // data
  int x;
  int y;

  Walker() { // constructor
  x = width/2;
  y = width/2;
  }

  void display() { // Objects have functions
  stroke(0);
  point(x,y);
  }

    void step() {
    int choice = int(random(4)); // Translate number to step
    if (choice == 0) {
      x++;
    } else if (choice == 1) {
      x--;
    } else if (choice == 2) {
      y++;
    } else {
    y--;
    }
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
    w.display();
  }