#include <iostream>
#include <vector>

int main(void) {
    const int arr[] = {1,2,3,4,5};
    std::vector<int> v(arr, arr + sizeof(arr)/sizeof(arr[0]));

    typedef std::vector<int>::const_iterator iter;
    iter b = v.begin();
    iter e = v.end();

    while(b != e) {
        std::cout << *b << std::endl;
        ++b;
    }

    return 0;
}
