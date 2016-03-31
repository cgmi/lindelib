#include "../include/linde/Segmentation.h"
#include "../include/linde/Color.h"

#include <glm/gtx/norm.hpp>
#include <stack>
#include <set>

// https://github.com/qiankanglai/opencv.meanshift
namespace MeanShiftHelpers
{

inline float color_distance( const glm::vec3 & a, const glm::vec3 & b)
{
    return glm::distance2(a, b);
}

inline int spatial_distance( const cv::Point& q, const cv::Point& p )
{
    int a = q.x-p.x, b=q.y-p.y;
    return a*a+b*b;
}

inline int getLabel( std::vector<int>& unionfind, int l )
{
    int r = unionfind[l];
    if(unionfind[r] == r)
        return r;
    else
    {
        unionfind[l] = getLabel(unionfind, unionfind[r]);
        return unionfind[l];
    }
}

inline int getLabel2( std::vector<int>& unionfind, int l )
{
    int r = unionfind[l];
    if(r<0)
        return r;
    else
    {
        unionfind[l] = getLabel2(unionfind, r);
        return unionfind[l];
    }
}

// RAList from EDISON

class RAList
{
    // This is cut from Mean Shift Analysis Library, Implemented by Chris M. Christoudias, Bogdan Georgescu
public:
    int		label;
    RAList	*next;
    RAList( void )
    {
        label			= -1;
        next			= 0;	//NULL
    }

    ~RAList( void )
    {

    }

    int Insert(RAList* entry)
    {
        if(!next)
        {
            next		= entry;
            entry->next = 0;
            return 0;
        }
        if(next->label > entry->label)
        {
            entry->next	= next;
            next		= entry;
            return 0;
        }
        exists	= 0;
        cur		= next;
        while(cur)
        {
            if(entry->label == cur->label)
            {
                exists = 1;
                break;
            }
            else if((!(cur->next))||(cur->next->label > entry->label))
            {
                entry->next	= cur->next;
                cur->next	= entry;
                break;
            }
            cur = cur->next;
        }
        return (int)(exists);
    }

private:
    ///////current and previous pointer/////
    RAList	*cur, *prev;
    unsigned char exists;

};

void filterStep(const cv::Mat_<glm::vec3>& img, cv::Mat_<glm::vec3> &result, float spatial_radius, float color_radius2)
{
    for(int i = 0; i < img.rows; i++)
    {
        for(int j = 0; j < img.cols; j++)
        {
            int ic = i;
            int jc = j;
            int icOld, jcOld;
            float LOld, AOld, BOld;
            glm::vec3 Lab = result(i, j);

            double shift = 5;
            for (int iters=0;shift > 3 && iters < 100;iters++)
            {
                icOld = ic;
                jcOld = jc;
                LOld = Lab.x;
                AOld = Lab.y;
                BOld = Lab.z;

                float mi = 0;
                float mj = 0;
                float mL = 0;
                float mU = 0;
                float mV = 0;
                int num=0;

                int i2from = glm::max(0.f,i-spatial_radius), i2to = glm::min<float>(img.rows, i+spatial_radius+1.f);
                int j2from = glm::max(0.f,j-spatial_radius), j2to = glm::min<float>(img.cols, j+spatial_radius+1.f);
                for (int i2 = i2from; i2 < i2to;i2++)
                {
                    for (int j2 = j2from; j2 < j2to; j2++)
                    {
                        glm::vec3 Lab2 = result(i2, j2);

                        const float dL = Lab2.x - Lab.x;
                        const float dA = Lab2.y - Lab.y;
                        const float dB = Lab2.z - Lab.z;
                        if (dL*dL+dA*dA+dB*dB <= color_radius2)
                        {
                            mi += i2;
                            mj += j2;
                            mL += Lab2.x;
                            mU += Lab2.y;
                            mV += Lab2.z;
                            num++;
                        }
                    }
                }
                float num_ = 1.f/num;
                Lab.x = mL*num_;
                Lab.y = mU*num_;
                Lab.z = mV*num_;
                ic = (int) (mi*num_+0.5);
                jc = (int) (mj*num_+0.5);
                int di = ic-icOld;
                int dj = jc-jcOld;
                float dL = Lab.x-LOld;
                float dA = Lab.y-AOld;
                float dB = Lab.z-BOld;

                shift = di*di+dj*dj+dL*dL+dA*dA+dB*dB;
            }
            result(i, j) = Lab;
        }
    }

}

int clusteringStep(std::vector<int> &modePointCounts, cv::Mat_<int>& labels, cv::Mat_<glm::vec3> &result,
                   const cv::Mat_<glm::vec3>& img, std::vector<glm::vec3> &mode, float color_radius2)
{
    int regionCount = 0;
    const int size = img.rows*img.cols;
    for (int l = 0; l < size; l++)
    {
        labels(l) = -1;
    }

    int label = -1;
    for(int i = 0; i < img.rows; i++)
    {
        for(int j = 0; j < img.cols; j++)
        {
            if(labels(i, j) < 0)
            {
                labels(i, j) = ++label;

                mode[label] = result(i, j);

                // Fill
                std::stack<cv::Point> neighStack;
                neighStack.push(cv::Point(j, i));
                const int dxdy[][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};

                while(!neighStack.empty())
                {
                    cv::Point p = neighStack.top();
                    neighStack.pop();

                    for(int k = 0; k < 8; k++)
                    {
                        int i2 = p.y + dxdy[k][0], j2 = p.x + dxdy[k][1];

                        if(i2 >= 0 && j2 >= 0
                                && i2 < img.rows && j2 < img.cols
                                && labels(i2, j2) < 0
                                && color_distance(result(i, j), result(i2, j2)) < color_radius2)
                        {
                            labels(i2, j2) = label;
                            neighStack.push(cv::Point(j2, i2));
                            modePointCounts[label]++;
                            mode[label] += result(i2, j2);
                        }
                    }
                }
                mode[label] /= (float)modePointCounts[label];
            }
            //current Region count
            regionCount = label+1;
        }
    }

    std::cout << "Mean Shift(Connect): " << regionCount << std::endl;

    return regionCount;
}

void transitiveClosure(std::vector<glm::vec3> &mode, cv::Mat_<int>& labels, const cv::Mat_<glm::vec3>& img,
                       std::vector<int> & modePointCounts, int & regionCount,
                       const float color_radius2)
{
    int oldRegionCount = regionCount;

    for(int counter = 0, deltaRegionCount = 1; counter<5 && deltaRegionCount>0; counter++)
    {
        // 1.Build RAM using classifiction structure
        RAList *raList = new RAList [regionCount], *raPool = new RAList [10*regionCount];	//10 is hard coded!
        for(int i = 0; i < regionCount; i++)
        {
            raList[i].label = i;
            raList[i].next = NULL;
        }
        for(int i = 0; i < regionCount*10-1; i++)
        {
            raPool[i].next = &raPool[i+1];
        }
        raPool[10*regionCount-1].next = NULL;
        RAList	*raNode1, *raNode2, *oldRAFreeList, *freeRAList = raPool;
        for(int i=0;i<img.rows;i++)
            for(int j=0;j<img.cols;j++)
            {
                if(i>0 && labels(i, j)!=labels(i-1, j))
                {
                    // Get 2 free node
                    raNode1			= freeRAList;
                    raNode2			= freeRAList->next;
                    oldRAFreeList	= freeRAList;
                    freeRAList		= freeRAList->next->next;
                    // connect the two region
                    raNode1->label	= labels(i, j);
                    raNode2->label	= labels(i-1, j);
                    if(raList[labels(i, j)].Insert(raNode2))	//already exists!
                        freeRAList = oldRAFreeList;
                    else
                        raList[labels(i-1, j)].Insert(raNode1);
                }
                if(j>0 && labels(i, j)!=labels(i, j-1))
                {
                    // Get 2 free node
                    raNode1			= freeRAList;
                    raNode2			= freeRAList->next;
                    oldRAFreeList	= freeRAList;
                    freeRAList		= freeRAList->next->next;
                    // connect the two region
                    raNode1->label	= labels(i, j);
                    raNode2->label	= labels(i, j-1);
                    if(raList[labels(i, j)].Insert(raNode2))
                        freeRAList = oldRAFreeList;
                    else
                        raList[labels(i, j-1)].Insert(raNode1);
                }
            }

        // 2.Treat each region Ri as a disjoint set
        for(int i = 0; i < regionCount; i++)
        {
            RAList	*neighbor = raList[i].next;
            while(neighbor)
            {
                if(color_distance(mode[i], mode[neighbor->label]) < color_radius2)
                {
                    int iCanEl = i, neighCanEl	= neighbor->label;
                    while(raList[iCanEl].label != iCanEl) iCanEl = raList[iCanEl].label;
                    while(raList[neighCanEl].label != neighCanEl) neighCanEl = raList[neighCanEl].label;
                    if(iCanEl<neighCanEl)
                        raList[neighCanEl].label = iCanEl;
                    else
                    {
                        //raList[raList[iCanEl].label].label = iCanEl;
                        raList[iCanEl].label = neighCanEl;
                    }
                }
                neighbor = neighbor->next;
            }
        }
        // 3. Union Find
        for(int i = 0; i < regionCount; i++)
        {
            int iCanEl	= i;
            while(raList[iCanEl].label != iCanEl) iCanEl	= raList[iCanEl].label;
            raList[i].label	= iCanEl;
        }
        // 4. Traverse joint sets, relabeling image.
        std::vector<int> modePointCounts_buffer(regionCount, 0);
        std::vector<glm::vec3> mode_buffer(regionCount);
        std::vector<int>	label_buffer(regionCount);

        for(int i=0;i<regionCount; i++)
        {
            label_buffer[i]	= -1;
            mode_buffer[i].x = 0.f;
            mode_buffer[i].y = 0.f;
            mode_buffer[i].z = 0.f;
        }
        for(int i=0;i<regionCount; i++)
        {
            int iCanEl	= raList[i].label;
            modePointCounts_buffer[iCanEl] += (float)modePointCounts[i];
            mode_buffer[iCanEl] += mode[i]*(float)modePointCounts[i];
        }
        int	label = -1;
        for(int i = 0; i < regionCount; i++)
        {
            int iCanEl	= raList[i].label;
            if(label_buffer[iCanEl] < 0)
            {
                label_buffer[iCanEl]	= ++label;

                mode[label]	= (mode_buffer[iCanEl])/((float)modePointCounts_buffer[iCanEl]);

                modePointCounts[label]	= modePointCounts_buffer[iCanEl];
            }
        }
        regionCount = label+1;
        for(int i = 0; i < img.rows; i++)
            for(int j = 0; j < img.cols; j++)
                labels(i, j)	= label_buffer[raList[labels(i, j)].label];

        //Destroy RAM
        delete[] raList;
        delete[] raPool;

        deltaRegionCount = oldRegionCount - regionCount;
        oldRegionCount = regionCount;
        std::cout << "Mean Shift(TransitiveClosure): " << regionCount << std::endl;
    }
}

void prune(cv::Mat_<int>& labels, std::vector<glm::vec3> & mode, const int minRegion, const cv::Mat_<glm::vec3>& img, std::vector<int> & modePointCounts, int& regionCount)
{
    std::vector<int> modePointCounts_buffer(regionCount, 0);
    std::vector<glm::vec3> mode_buffer(regionCount);
    std::vector<int>	label_buffer(regionCount);
    int minRegionCount;

    do{
        minRegionCount = 0;
        // Build RAM again
        RAList *raList = new RAList [regionCount], *raPool = new RAList [10*regionCount];	//10 is hard coded!
        for(int i = 0; i < regionCount; i++)
        {
            raList[i].label = i;
            raList[i].next = NULL;
        }
        for(int i = 0; i < regionCount*10-1; i++)
        {
            raPool[i].next = &raPool[i+1];
        }
        raPool[10*regionCount-1].next = NULL;
        RAList	*raNode1, *raNode2, *oldRAFreeList, *freeRAList = raPool;
        for(int i=0;i<img.rows;i++)
            for(int j=0;j<img.cols;j++)
            {
                if(i>0 && labels(i, j)!=labels(i-1, j))
                {
                    // Get 2 free node
                    raNode1			= freeRAList;
                    raNode2			= freeRAList->next;
                    oldRAFreeList	= freeRAList;
                    freeRAList		= freeRAList->next->next;
                    // connect the two region
                    raNode1->label	= labels(i, j);
                    raNode2->label	= labels(i-1, j);
                    if(raList[labels(i, j)].Insert(raNode2))	//already exists!
                        freeRAList = oldRAFreeList;
                    else
                        raList[labels(i-1, j)].Insert(raNode1);
                }
                if(j>0 && labels(i, j)!=labels(i, j-1))
                {
                    // Get 2 free node
                    raNode1			= freeRAList;
                    raNode2			= freeRAList->next;
                    oldRAFreeList	= freeRAList;
                    freeRAList		= freeRAList->next->next;
                    // connect the two region
                    raNode1->label	= labels(i, j);
                    raNode2->label	= labels(i, j-1);
                    if(raList[labels(i, j)].Insert(raNode2))
                        freeRAList = oldRAFreeList;
                    else
                        raList[labels(i, j-1)].Insert(raNode1);
                }
            }
        // Find small regions
        for(int i = 0; i < regionCount; i++)
            if(modePointCounts[i] < minRegion)
            {
                minRegionCount++;
                RAList *neighbor = raList[i].next;
                int candidate = neighbor->label;
                float minDistance = color_distance(mode[i], mode[candidate]);
                neighbor = neighbor->next;
                while(neighbor)
                {
                    float minDistance2 = color_distance(mode[i], mode[neighbor->label]);
                    if(minDistance2<minDistance)
                    {
                        minDistance = minDistance2;
                        candidate = neighbor->label;
                    }
                    neighbor = neighbor->next;
                }
                int iCanEl = i, neighCanEl	= candidate;
                while(raList[iCanEl].label != iCanEl) iCanEl = raList[iCanEl].label;
                while(raList[neighCanEl].label != neighCanEl) neighCanEl = raList[neighCanEl].label;
                if(iCanEl < neighCanEl)
                    raList[neighCanEl].label	= iCanEl;
                else
                {
                    //raList[raList[iCanEl].label].label	= neighCanEl;
                    raList[iCanEl].label = neighCanEl;
                }
            }
        for(int i = 0; i < regionCount; i++)
        {
            int iCanEl	= i;
            while(raList[iCanEl].label != iCanEl)
                iCanEl	= raList[iCanEl].label;
            raList[i].label	= iCanEl;
        }
        memset(modePointCounts_buffer.data(), 0, regionCount*sizeof(int));
        for(int i = 0; i < regionCount; i++)
        {
            label_buffer[i]     = -1;
            mode_buffer[i].x	= 0.f;
            mode_buffer[i].y	= 0.f;
            mode_buffer[i].z	= 0.f;
        }
        for(int i=0;i<regionCount; i++)
        {
            int iCanEl	= raList[i].label;
            modePointCounts_buffer[iCanEl] += (float)modePointCounts[i];
            mode_buffer[iCanEl] += mode[i]*(float)modePointCounts[i];
        }
        int	label = -1;
        for(int i = 0; i < regionCount; i++)
        {
            int iCanEl	= raList[i].label;
            if(label_buffer[iCanEl] < 0)
            {
                label_buffer[iCanEl]	= ++label;

                mode[label]	= (mode_buffer[iCanEl])/((float)modePointCounts_buffer[iCanEl]);

                modePointCounts[label]	= modePointCounts_buffer[iCanEl];
            }
        }
        regionCount = label+1;
        for(int i = 0; i < img.rows; i++)
            for(int j = 0; j < img.cols; j++)
                labels(i, j)	= label_buffer[raList[labels(i, j)].label];

        //Destroy RAM
        delete[] raList;
        delete[] raPool;

        std::cout<<"Mean Shift(Prune):"<<regionCount<<std::endl;

    } while(minRegionCount > 0);
}

} // namespace MeanShiftHelpers


namespace linde
{

int MeanShift(const cv::Mat_<glm::vec3>& img, cv::Mat_<int> & labels, const int hs, const float hc, const int minRegion)
{
    const float color_radius2=hc*hc;

    // use Lab rather than L*u*v!
    // since Luv may produce noise points
    cv::Mat_<glm::vec3> result(img.size());
    convert_image(img, result, convert_rgb2lab);


    // Step One. Filtering stage of meanshift segmentation
    // http://rsbweb.nih.gov/ij/plugins/download/Mean_Shift.java
    MeanShiftHelpers::filterStep(img, result, hs, color_radius2);

    // Step Two. Cluster
    // Connect
    const int size  =       img.rows*img.cols;
    std::vector<int>        modePointCounts(size, 0);
    std::vector<glm::vec3>  mode(size);
    if (labels.size() != img.size())
    {
        labels.create(img.size());
    }
    int regionCount = MeanShiftHelpers::clusteringStep(modePointCounts, labels, result, img, mode, color_radius2);

    // TransitiveClosure
    MeanShiftHelpers::transitiveClosure(mode, labels, img, modePointCounts, regionCount, color_radius2);

    // Prune
    MeanShiftHelpers::prune(labels, mode, minRegion, img, modePointCounts, regionCount);

    // Output
    std::cout <<"Mean Shift ready:" << std::endl;

    return regionCount;
}


// true for boundaries
int ConnectedComponents(const cv::Mat_<uchar> &binary, cv::Mat_<int> & labels)
{
    const int NOT_LABELED = -2;
    const int BOUNDARY = -1;

    if (binary.size() != labels.size())
        labels.create(binary.size());
    for (int i = 0; i < binary.rows*binary.cols; i++)
    {
        if (binary(i))
        {
            labels(i) = NOT_LABELED;
        } else
        {
            labels(i) = BOUNDARY;
        }
    }

    int label_count = 0;

    for(int y=0; y < binary.rows; y++)
    {
        for(int x=0; x < binary.cols; x++)
        {
            if(labels(y,x) != NOT_LABELED)
            {
                continue;
            }

            cv::Rect rect;
            cv::floodFill(labels, cv::Point(x,y), cv::Scalar(label_count), &rect, cv::Scalar(0), cv::Scalar(0), 4);

            label_count++;
        }
    }

    // clear left over boundaries
    std::function<bool(const cv::Mat_<int> & labels)> boundaryLeft = [](const cv::Mat_<int> & labels)
    {
        for (uint i = 0; i < labels.total(); i++)
        {
            if (labels(i) == BOUNDARY) return true;
        }
        return false;
    };

    while (boundaryLeft(labels))
    {
        const int r = 3;
        for(int y=0; y < binary.rows; y++)
        {
            for(int x=0; x < binary.cols; x++)
            {
                if(labels(y,x) != BOUNDARY)
                {
                    continue;
                }
                std::map<int, int> count;
                for(int i = y-r; i <= y+r; i++)
                {
                    for(int j = x-r; j <= x+r; j++)
                    {
                        if (i < 0 || i >= labels.rows || j < 0 || j >= labels.cols) continue;

                        if(labels(i, j) == BOUNDARY)
                        {
                            continue;
                        } else
                        {
                            if (count.find(labels(i, j)) == count.end())
                            {
                                count[labels(i, j)] = 0;
                            } else
                            {
                                count[labels(i, j)]++;
                            }
                        }

                    }
                }
                int maxV = 0;
                for (std::map<int,int>::iterator it=count.begin(); it!=count.end(); ++it)
                {
                    maxV = glm::max(maxV, it->second);
                }
                labels(y, x) = maxV;
            }
        }
    }


    return label_count;
}


UCMParser::UCMParser():
    m_ucm()
{

}

UCMParser::UCMParser(const cv::Mat_<float> &ucm) :
    m_ucm()
{
    parse(ucm);
}

UCMParser::~UCMParser()
{

}

void UCMParser::parse(const cv::Mat_<float> &ucm)
{
    m_ucm = ucm;
    parseLevels(m_levels);
}

void UCMParser::getLevels(std::vector<float> & levels) const
{
    levels.clear();
    levels.insert(levels.begin(), m_levels.cbegin(), m_levels.cend());
}

int UCMParser::getLevel(const float level, cv::Mat_<int> &labels) const
{
    cv::Mat_<uchar> binary;
    binary.create(m_ucm.size());
    if (labels.size() != m_ucm.size())
    {
        labels.create(m_ucm.size());
    }
    for (int i = 0; i < m_ucm.rows*m_ucm.cols; i++)
    {
        if (m_ucm(i) < level)
        {
            binary(i) = 255;
        } else
        {
            binary(i) = 0;
        }
    }

    const int nr = ConnectedComponents(binary, labels);

    return nr;
}

void UCMParser::parseLevels(std::set<float> &levels) const
{
    levels.clear();
    for (int i = 0; i < m_ucm.rows*m_ucm.cols; i++)
    {
        if (m_ucm(i) > 0.f)
        {
            levels.insert(m_ucm(i));
        }
    }
}

void UnsharpMasking(const cv::Mat_<float> &depth, cv::Mat_<float> &spatialImportanceFunction, const float sigma)
{
    spatialImportanceFunction.create(depth.size());

    cv::GaussianBlur(depth, spatialImportanceFunction, cv::Size(-1, -1), sigma);

    spatialImportanceFunction = spatialImportanceFunction - depth;
}

} // namespace linde
