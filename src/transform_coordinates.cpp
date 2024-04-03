// Gets as input x and y coordinates and checks if the coordinates are outside of the window
// Translates the X,Y coordinates into the entry of the 1D array
// N is the max size, i.e. here SIZE*SIZE
int transform_coordinates(int x, int y, int N) {
	if (x < 0) 
                x=0; 
        else if (x > N-1) 
                x=N-1; 
  
        if (y < 0) 
                y=0; 
        if (y > N-1) 
                y=N-1;
  
	return (y * N) + x;
}

