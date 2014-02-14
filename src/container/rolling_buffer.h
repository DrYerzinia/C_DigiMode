
class rolling_buffer {

	rolling_buffer(int s);

	~rolling_buffer();

private:

	int size;
	int current_start;

	char *buffer;

};
