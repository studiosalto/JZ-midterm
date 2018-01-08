class Walker {
int x;
int y;

Walker() {
  x = width/2;
  y = width/2;
}

void display() {
  stroke(0);
  point(x,y);
}

void step() {
  int choice = int(random(4));
}

if (choice == 0)
  x++;
} else if (choice == 1) {
  x--;
} else if (choice == 2) {
  y++;
} else {
  y--;
}

void setup() {
  size(640,360);
  //create a Walker
  w = new Walker();
  background(255);
}
void draw() {
  w.step();
  w.display();
}
