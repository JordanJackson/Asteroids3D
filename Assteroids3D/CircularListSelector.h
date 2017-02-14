#ifndef CIRCULAR_LIST_SELECTOR_H_
#define CIRCULAR_LIST_SELECTOR_H_

template <typename T>
class CircularListSelector {
    std::vector<T>      mItems;
    unsigned            mCurrIndex;

public:

    CircularListSelector()
        : mItems()
        , mCurrIndex(0)
    { }

    void addItem(const T& item)
    { mItems.push_back(item); }

    bool isEmpty() const
    { return mItems.empty(); }

    //
    // random access stuff
    //

    unsigned numItems() const
    { return (unsigned)mItems.size(); }

    const T& getItem(unsigned index) const
    { return mItems[index]; }

    void selectItem(unsigned index)
    {
        if (index < mItems.size()) {
            mCurrIndex = index;
        }
    }

    //
    // sequential access stuff
    //

    const T& getSelectedItem() const
    { return mItems[mCurrIndex]; }

    void moveNext()
    {
        if (!mItems.empty()) {
            ++mCurrIndex;
            if (mCurrIndex >= mItems.size()) {
                mCurrIndex = 0;
            }
        }
    }

    void movePrev()
    {
        if (!mItems.empty()) {
            if (mCurrIndex > 0) {
                --mCurrIndex;
            } else {
                mCurrIndex = mItems.size() - 1;
            }
        }
    }
};

#endif
