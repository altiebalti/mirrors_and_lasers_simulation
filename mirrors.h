class Mirror {
public:
	double x;
	double y;
	double angle;
	int length;
    Mirror():Mirror(0, 0, 0, 50){

    }
    Mirror(double x, double y, double angle, int length){
    	this->x = x;
    	this->y = y;
    	this->angle = angle;
    	this->length = length;
    }
};
