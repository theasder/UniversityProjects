#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <vector>
#include <tuple>
#include <queue>
#include <cmath>
#include <algorithm>

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::vector;
using std::tuple;
using std::tie;
using std::make_tuple;
using std::queue;
using std::sort;
using std::abs;

#include "io.h"
#include "matrix.h"

typedef tuple<uint, uint, uint, uint> Rect;


void drawline(Image& a, int x1, int y1, int x2, int y2)
{
    // Bresenham's line algorithm
    const bool steep = (abs(y2 - y1) > abs(x2 - x1));
    Image b = a;
    
    if(steep)
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }
    
    if(x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    
    const int dx = x2 - x1;
    const int dy = abs(y2 - y1);
    
    int error = dx / 2;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = y1;
    
    const int maxX = x2;
    
    for(int x = x1; x < maxX; x++)
    {
        if(steep)
        {
            b(y, x) = make_tuple(30, 200, 30);
        }
        else
        {
            b(x, y) = make_tuple(30, 200, 30);
        }
        
        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}

void drawrect(Image& a, uint x, uint y, uint width, uint height)
{
    Image b = a;
    drawline(b, x, y, x, y + height);
    drawline(b, x, y, x + width, y);
    drawline(b, x + width, y, x + width, y + height);
    drawline(b, x, y + height, x + width, y + height);
}

tuple<uint, uint, uint, uint, uint, uint, signed long long int, signed long long int> fill2(Matrix<uint>& a, uint i, uint j, int label, Image& im)
{
    queue<tuple<int, int>> coord;
    uint x = i, y = j;
    uint min_x = i, min_y = j, max_x = i, max_y = j, area = 0, perimeter = 0;
    // defining border pixel
    uint flag = 0;
    signed long long int center_of_masses_x = 0, center_of_masses_y = 0;
    Image ik = im;
    coord.push(make_tuple(i, j));
    
    while(!coord.empty())
    {
        flag = 0;
        tie(x, y) = coord.front();
        a(x, y) = label;
        
        center_of_masses_x += x;
        center_of_masses_y += y;
        if(x < min_x)
            min_x = x;
        if(y < min_y)
            min_y = y;
        if(x > max_x)
            max_x = x;
        if(y > max_y)
            max_y = y;
        area++;
        
        im(x, y) = make_tuple(14 * label, 13 * label, 16 * label);
        coord.pop();
        
        if(x > 0 && a(x - 1, y) == 1)
        {
            coord.push(make_tuple(x - 1, y));
            a(x - 1, y) = 2;
        }
        
        if(x > 0 && a(x - 1, y) == 0)
            flag = 1;
        
        if(y > 0 && a(x, y - 1) == 1)
        {
            coord.push(make_tuple(x, y - 1));
            a(x, y - 1) = 2;
        }
        
        if(y > 0 && a(x, y - 1) == 0)
            flag = 1;
        
        if(x > 0 && y > 0 && a(x - 1, y - 1) == 1)
        {
            coord.push(make_tuple(x - 1, y - 1));
            a(x - 1, y - 1) = 2;
        }
        
        if(x > 0 && y > 0 && a(x - 1, y - 1) == 0)
            flag = 1;
        
        if(x > 0 && y < a.n_cols - 1 && a(x - 1, y + 1) == 1)
        {
            coord.push(make_tuple(x - 1, y + 1));
            a(x - 1, y + 1) = 2;
        }
        
        if(x > 0 && y < a.n_cols - 1 && a(x - 1, y + 1) == 0)
            flag = 1;
        
        if(y > 0 && x < a.n_rows && a(x + 1, y - 1) == 1)
        {
            coord.push(make_tuple(x + 1, y - 1));
            a(x + 1, y - 1) = 2;
        }
        
        if(y > 0 && x < a.n_rows && a(x + 1, y - 1) == 0)
            flag = 1;
        
        if(x < a.n_rows - 1 && a(x + 1, y) == 1)
        {
            coord.push(make_tuple(x + 1, y));
            a(x + 1, y) = 2;
        }
        
        if(x < a.n_rows - 1 && a(x + 1, y) == 0)
            flag = 1;
        
        if(y < a.n_cols - 1 && a(x, y + 1) == 1)
        {
            coord.push(make_tuple(x, y + 1));
            a(x, y + 1) = 2;
        }
        
        if(y < a.n_cols - 1 && a(x, y + 1) == 0)
            flag = 1;
        
        if(x < a.n_rows - 1 && y < a.n_cols - 1 && a(x + 1, y + 1) == 1)
        {
            coord.push(make_tuple(x + 1, y + 1));
            a(x + 1, y + 1) = 2;
        }
        
        if(x < a.n_rows - 1 && y < a.n_cols - 1 && a(x + 1, y + 1) == 0)
            flag = 1;
        
        if(flag) {
            perimeter++;
            ik(x, y) = make_tuple(0, 0, 255);
        }
    }
    
    center_of_masses_x /= area;
    center_of_masses_y /= area;
    //cout << "cmx " << center_of_masses_x << endl;
    //cout << "cmy " << center_of_masses_y << endl;
    
    //drawrect(ik, center_of_masses_x - 5, center_of_masses_y - 5, 10, 10);
    return make_tuple(min_x, min_y, max_x, max_y, area, perimeter, center_of_masses_x, center_of_masses_y);
}


tuple<signed long long int, signed long long int, signed long long int> statical_moment(Matrix<uint>& a, uint i, uint j, signed long long int center_of_masses_x, signed long long int center_of_masses_y)
{
    queue<tuple<int, int>> coord;
    uint x = 0, y = 0;
    signed long long int m20 = 0, m02 = 0, m11 = 0;
    // defining border pixel
    
    coord.push(make_tuple(i, j));
    
    while(!coord.empty())
    {
        tie(x, y) = coord.front();
        
        m20 += (static_cast<int>(x) - static_cast<int>(center_of_masses_x)) * (static_cast<int>(x) - static_cast<int>(center_of_masses_x));
        m02 += (static_cast<int>(y) - static_cast<int>(center_of_masses_y)) * (static_cast<int>(y) - static_cast<int>(center_of_masses_y));
        m11 += (static_cast<int>(x) - static_cast<int>(center_of_masses_x)) * (static_cast<int>(y) - static_cast<int>(center_of_masses_y));
    
        coord.pop();
        
        if(x > 0 && a(x - 1, y) == 1)
        {
            coord.push(make_tuple(x - 1, y));
            a(x - 1, y) = 2;
        }
        
        if(y > 0 && a(x, y - 1) == 1)
        {
            coord.push(make_tuple(x, y - 1));
            a(x, y - 1) = 2;
        }
        
        if(x > 0 && y > 0 && a(x - 1, y - 1) == 1)
        {
            coord.push(make_tuple(x - 1, y - 1));
            a(x - 1, y - 1) = 2;
        }
        
        if(x > 0 && y < a.n_cols - 1 && a(x - 1, y + 1) == 1)
        {
            coord.push(make_tuple(x - 1, y + 1));
            a(x - 1, y + 1) = 2;
        }
        
        if(y > 0 && x < a.n_rows && a(x + 1, y - 1) == 1)
        {
            coord.push(make_tuple(x + 1, y - 1));
            a(x + 1, y - 1) = 2;
        }
        
        if(x < a.n_rows - 1 && a(x + 1, y) == 1)
        {
            coord.push(make_tuple(x + 1, y));
            a(x + 1, y) = 2;
        }
        
        if(y < a.n_cols - 1 && a(x, y + 1) == 1)
        {
            coord.push(make_tuple(x, y + 1));
            a(x, y + 1) = 2;
        }
        
        if(x < a.n_rows - 1 && y < a.n_cols - 1 && a(x + 1, y + 1) == 1)
        {
            coord.push(make_tuple(x + 1, y + 1));
            a(x + 1, y + 1) = 2;
        }
    }
    
    return make_tuple(m20, m02, m11);
}

/*
int otsuThreshold(Matrix<uint>& im)
{
    int min = im(0,0), max = im(0, 0);
    int temp, temp1;
    //int *hist;
    int histSize;
    
    int alpha, beta, threshold = 0;
    double sigma, maxSigma = -1;
    double w1, a;
    
    // Построение гистограммы ****
    // Узнаем наибольший и наименьший полутон
    for (uint i = 0; i < im.n_rows; i++)
        for(uint j = 0; j < im.n_cols; j++)
        {
            temp = im(i, j);
            if(temp < min)
                min = temp;
            if(temp > max)
                max = temp;
        }
    
    histSize = max - min + 1;
    //if((hist=(int*) malloc(sizeof(int)*histSize))==NULL) return -1;
    int *hist = new int[histSize];
    
    for(int i = 0; i < histSize; i++)
        hist[i] = 0;
    
    // Считаем сколько каких полутонов
    for (uint i = 0; i < im.n_rows; i++)
        for(uint j = 0; j < im.n_cols; j++)
        {
            hist[im(i, j) - min]++;
        }
    
    // Гистограмма построена
    
    temp = temp1 = 0;
    alpha = beta = 0;
    // Для расчета математического ожидания первого класса
    for(int i = 0; i <= (max - min); i++)
    {
        temp += i * hist[i];
        temp1 += hist[i];
    }
    
    // Основной цикл поиска порога
    // Пробегаемся по всем полутонам для поиска такого, при котором внутриклассовая дисперсия минимальна
    for(int i = 0; i < (max - min); i++)
    {
        alpha += i * hist[i];
        beta += hist[i];
        
        w1 = beta / temp1;
        a =  alpha / beta - (temp - alpha) / (temp1 - beta);
        sigma = w1 * (1 - w1) * a * a;
        
        if(sigma > maxSigma)
        {
            maxSigma = sigma;
            threshold = i;
        }
    }
    
    delete hist;
    
    return threshold + min;
}


tuple<uint, uint> binarize(Matrix<uint>& a, Image& in)
{
    Image im = in;
    uint r, g, b;
    uint point_x, point_y;
    int threshold = otsuThreshold(a);
    
    for (uint i = 0; i < im.n_rows; i++)
        for(uint j = 0; j < im.n_cols; j++)
        {
            tie(r, g, b) = im(i, j);
            if(static_cast<int>(a(i, j)) > threshold + 17)
            {
                if(r > 240 && g < 10 && b < 10)
                {
                    point_x = i;
                    point_y = j;
                }
                im(i, j) = make_tuple(0, 0, 0);
                a(i, j) = 1;
            }
            else
            {
                im(i, j) = make_tuple(255, 255, 255);
                a(i, j) = 0;
            }
        }
    return make_tuple(point_x, point_y);
}
*/


tuple<uint, uint> binarize(Matrix<uint>& a, Image& in)
{
    Image im = in;
    uint r = 0, g = 0, b = 0;
    uint point_x = 0, point_y = 0;

    for (uint i = 0; i < im.n_rows; i++)
        for(uint j = 0; j < im.n_cols; j++)
        {
            tie(r, g, b) = im(i, j);
            if(r + g + b > 134)
            {
                if(r > 240 && g < 10 && b < 10)
                {
                    point_x = i;
                    point_y = j;
                }
                im(i, j) = make_tuple(0, 0, 0);
                a(i, j) = 1;
            }
            else
            {
                im(i, j) = make_tuple(255, 255, 255);
                a(i, j) = 0;
            }
        }
    return make_tuple(point_x, point_y);
}



tuple<vector<tuple<uint, uint, uint, uint, uint, uint, signed long long int, signed long long int, double>>, int>
extract_connecting_components(Matrix<uint>& a, Image& in, uint point_x, uint point_y)
{
    Image im = in;
    int label = 2, begin_index = 0;
    auto objects = vector<tuple<uint, uint, uint, uint, uint, uint, signed long long int, signed long long int, double>>();
    uint min_x, min_y, max_x, max_y, area, perimeter;
    signed long long int center_of_masses_x = 0, center_of_masses_y = 0;
    double compact;
    
    for (uint i = 0; i < im.n_rows - 1; i++)
        for (uint j = 0; j < im.n_cols - 1; j++)
        {
            if(a(i, j) == 1)
            {
                ++label;
                tie(min_x, min_y, max_x, max_y, area, perimeter, center_of_masses_x, center_of_masses_y) = fill2(a, i, j, label, im);
                //cout << " area = " << area << " perimeter = " << perimeter << endl;
                if(min_x <= point_x && point_x <= max_x && point_y <= max_y && min_y <= point_y)
                    begin_index = label - 3;
                
                compact = (perimeter * perimeter + 0.0) / area;
                objects.push_back(make_tuple(min_x, min_y, max_x - min_x, max_y - min_y, area, perimeter, center_of_masses_x, center_of_masses_y, compact));
            }
        }
    return make_tuple(objects, begin_index);
}


tuple<vector<Rect>, Image>
find_treasure(const Image& in)
{
    // Base: return Rect of treasure only
    // Bonus: return Rects of arrows and then treasure Rect
    auto path = vector<Rect>();
    auto possible_treasure = vector<Rect>();
    auto statical_moments = vector<tuple<signed long long int, signed long long int, signed long long int>>();
    auto objects = vector<tuple<uint, uint, uint, uint, uint, uint, signed long long int, signed long long int, double>>();
    auto elongation = vector<double>();
    const double PI = 3.14159265358979;
    // image for binarization
    Image im = in.deep_copy();
    // image for drawing
    Image ik = in;
    // point of red arrow
    uint point_x = 0, point_y = 0;
    int begin_index = 0, index = 0;
    uint x = 0, y = 0, width = 0, height = 0;
    int label = 2;
    int flag = 0;
    uint area = 0, perimeter = 0;
    //uint min_x, min_y, max_x, max_y, area, perimeter;
    signed long long int center_of_masses_x = 0, center_of_masses_y = 0;
    double compact = 0.0;
    signed long long int m02 = 0, m20 = 0, m11 = 0;
    double elong = 0.0;
    uint r = 0, g = 0, b = 0;
    
    Matrix<uint> a(im.n_rows,im.n_cols);
    
    // binarization
    for (uint i = 0; i < im.n_rows - 1; i++)
        for (uint j = 0; j < im.n_cols - 1; j++)
        {
            tie(r, g, b) = im(i, j);
            a(i, j) = 0.299 * r + 0.587 * g + 0.114 * b;
        }
    
    
    tie(point_x, point_y) = binarize(a, im);
    Matrix<uint> d = a.deep_copy();
    Matrix<uint> l = a.deep_copy();
    
    //path.push_back(Rect(234,234,34,2));
    
    //extracting connected components
    tie(objects, begin_index) = extract_connecting_components(a, im, point_x, point_y);
    uint n = objects.size();
    
    //cout << "n = " << n << endl;
    
    // calculating statical moments
    uint k = 0;
    for (uint i = 0; i < im.n_rows - 1; i++)
        for (uint j = 0; j < im.n_cols - 1; j++)
        {
            if(d(i, j) == 1)
            {
                tie(x, y, width, height, area, perimeter, center_of_masses_x, center_of_masses_y, compact) = objects[k];
                k++;
                tie(m20, m02, m11) = statical_moment(d, i, j, center_of_masses_x, center_of_masses_y);
                //cout << m20 << " " << m02 << " " << m11 << endl;
                statical_moments.push_back(make_tuple(m20, m02, m11));
                elong = (m20 + m02 + 0.0) / (m20 + m02 - sqrt((m20 - m02) * (m20 - m02) + 4 * m11 * m11 + 0.0)) + ((sqrt((m20 - m02) * (m20 - m02) + 4 * m11 * m11 + 0.0)) / (m20 + m02 - sqrt((m20 - m02) * (m20 - m02) + 4 * m11 * m11 + 0.0)) + 0.0);
                
                elongation.push_back(elong);
                
            }
        }
    
    
    double* koeff = new double[n];
    double* koeff2 = new double[n];
    double* koeff_unsorted = new double[n];
    double* koeff2_unsorted = new double[n];
    
    for (uint i = 0; i < n; i++)
    {
        tie(x, y, width, height, area, perimeter, center_of_masses_x, center_of_masses_y, compact) = objects[i];
        koeff[i] = elongation[i];
        koeff2[i] = compact;
        koeff_unsorted[i] = koeff[i];
        koeff2_unsorted[i] = koeff2[i];
        //cout << "elongation: " << abs(koeff[i]) << " compact: " << abs(koeff2[i]) << endl;
    }
    //uint sum = 0.0;
    
    //for (uint i = 0; i < n; i++)
    //    sum += koeff[i];
    double mean = 0.0, mean2 = 0.0;
    
    sort(koeff, koeff + n);
    sort(koeff2, koeff2 + n);
    
    mean = koeff[n / 2];
    mean2 = koeff2[n / 2];
    //mean = sum / n;
    //cout << "1.3 * mean = " << 1.3 * mean << endl;
    //cout << "0.7 * mean = " << 0.7 * mean << endl;
    //cout << "1.6 * mean2 = " << 1.6 * mean2 << endl;
    //cout << "0.4 * mean = " << 0.4 * mean2 << endl;
    
    for (uint i = 0; i < n; i++)
    {
        if((abs(koeff_unsorted[i] - mean) > 0.3 * mean) || (abs(koeff2_unsorted[i] - mean2) > 0.4 * mean2))
        //if(koeff[i] > 1.1 * mean || koeff[i] < 0.7 * mean)
        {
            //cout << koeff_unsorted[i] << endl;
            tie(x, y, width, height, area, perimeter, center_of_masses_x, center_of_masses_y, compact) = objects[i];
            possible_treasure.push_back(make_tuple(x, y, width, height));
            drawrect(im, x, y, width, height);
            //path.push_back(make_tuple(y, x, width, height));
        }
    }
    //save_image(im, "pic/test5.bmp");
    
    
    for (uint i = 0; i < possible_treasure.size(); i++)
    {
        tie(x, y, width, height) = possible_treasure[i];
        //cout << "x = " << x << " y = " << y << " width = " << width << " height = " << height << " area = " << area << " perimeter = " << perimeter << " center of masses = (" << center_of_masses_x << ";" << center_of_masses_y << ") compact = " << compact << endl;
        
        drawrect(ik, x, y, width, height);
    }
    
    
    //path.push_back(make_tuple(y, x, width, height));
    
    tie(x, y, width, height, area, perimeter, center_of_masses_x, center_of_masses_y, compact) = objects[begin_index];
    //drawrect(ik, x, y, width, height);
    double angle = 0.0;
    double* angles = new double[n];
    double green_point_x = 0.0, green_point_y = 0.0;
    double distance1, distance2;
    
    for(uint i = 0; i < n; i++)
    {
        flag = 0;
        tie(m20, m02, m11) = statical_moments[i];
        //cout << m20 << " " << m02 << " " << m11 << endl;
        
        tie(x, y, width, height, area, perimeter, center_of_masses_x, center_of_masses_y, compact) = objects[i];
        
        for(uint j = x; j < x + width; j++)
            for(uint m = y; m < y + height; m++)
            {
                tie(r, g, b) = ik(j, m);
                if(g > 230 && b < 30)
                {
                    green_point_x = j;
                    green_point_y = m;
                    //drawrect(ik, j - 5, m - 5, 10, 10);
                }
            }
        
        //drawline(ik, green_point_x, green_point_y, center_of_masses_x, center_of_masses_y);
        
        angle = 0.5 * atan2(2 * m11, (m20 - m02));
        angles[i] = angle;
        
        distance1 = floor(center_of_masses_x + 20 * cos(angles[i]) - green_point_x) * floor(center_of_masses_x + 20 * cos(angles[i]) - green_point_x) + floor(center_of_masses_y + 20 * sin(angles[i]) - green_point_y) * floor(center_of_masses_y + 20 * sin(angles[i]) - green_point_y);
        
        distance2 = floor(center_of_masses_x + 20 * cos(angles[i] + PI) - green_point_x) * floor(center_of_masses_x + 20 * cos(angles[i] + PI) - green_point_x) + floor(center_of_masses_y + 20 * sin(angles[i] + PI) - green_point_y) * floor(center_of_masses_y + 20 * sin(angles[i] + PI) - green_point_y);
        
        
        if(distance1 > distance2)
        {
            angles[i] += PI;
        }
        
        //cout << angles[i] << endl;
        
        //angles[i] -= PI / 2;
        //cout << (angles[i] - PI / 2) * 180 / PI << endl;
        //drawline(ik, center_of_masses_x, center_of_masses_y, floor(center_of_masses_x + 20 * cos(angles[i])), floor(center_of_masses_y + 20 * sin(angles[i])));
        //cout << angle << endl;
        
    }
    
    index = begin_index;
    uint x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    label = 1;
    int counter = 0;
    
    while(label)
    {
        angle = angles[index];
        //cout << "koeff = " << angle << endl;
        counter = 0;
        
        
        tie(x, y, width, height, area, perimeter, center_of_masses_x, center_of_masses_y, compact) = objects[index];
        
        path.push_back(make_tuple(y, x, height, width));
        x0 = center_of_masses_x;
        y0 = center_of_masses_y;
        flag = 0;
        
        while(1)
        {
            x = floor(center_of_masses_x + counter * cos(angle));
            y = floor(center_of_masses_y + counter * sin(angle));
            
            if(y >= d.n_cols || x >= d.n_rows)
            {
                label = 0;
                break;
            }
            //cout << "x = " << x << " y = " << y << " d(x, y) = " << d(x, y) << " flag = " << flag << endl;
            if(flag == 0 && d(x, y) == 0)
                flag = 1;
            else if(flag == 1 && d(x, y) > 0)
                break;
            counter++;
        }
        
        for(uint i = 0; i < n; i++)
        {
            tie(x1, y1, width, height, area, perimeter, center_of_masses_x, center_of_masses_y, compact) = objects[i];
            
            if(x >= x1 && y >= y1 && x <= x1 + width && y <= y1 + height)
            {
                index = i;
                break;
            }
        }
        tie(x1, y1, width, height, area, perimeter, center_of_masses_x, center_of_masses_y, compact) = objects[index];
        drawline(ik, x0, y0, center_of_masses_x, center_of_masses_y);
        //cout << "draw line from " << x0 << " " << y0 << " to " << center_of_masses_x << " " << center_of_masses_y << endl;
        
        if(x0 == center_of_masses_x && y0 == center_of_masses_y)
        {
            label = 0;
            break;
        }
        
        for(uint i = 0; i < possible_treasure.size(); i++)
        {
            tie(x1, y1, width, height) = possible_treasure[i];
            if(center_of_masses_x >= x1 && center_of_masses_y >= y1 && center_of_masses_x <= x1 + width && center_of_masses_y <= y1 + height) {
                //drawline(ik, x1, y1, x0, x0);
                //cout << "draw line" << endl;
                path.push_back(make_tuple(y1, x1, height, width));
                label = 0;
                break;
            }
        }
        
    }
    delete angles;
    delete koeff;
    
    return make_tuple(path, in.deep_copy());
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        cout << "Usage: " << endl << argv[0]
             << " <in_image.bmp> <out_image.bmp> <out_path.txt>" << endl;
        return 0;
    }

    try {
        //Image src_image = load_image(argv[1]);
        Image src_image = load_image("pic/9_simple.bmp");

        ofstream fout(argv[3]);

        vector<Rect> path;
        Image dst_image;
        tie(path, dst_image) = find_treasure(src_image);
        save_image(dst_image, argv[2]);

        uint x, y, width, height;
        for (const auto &obj : path)
        {
            tie(x, y, width, height) = obj;
            fout << x << " " << y << " " << width << " " << height << endl;
        }

    } catch (const string &s) {
        cerr << "Error: " << s << endl;
        return 1;
    }
}
