#ifndef PAGEREFRESHER_H
#define PAGEREFRESHER_H

#include <QWidget>
#include <functional>
#include <QPushButton>

class PageRefresher
{
public:
    static void clearSearchFields(QWidget* container);
    
    static QPushButton* createRefreshButton(QWidget* parent, 
                                           const QString& text = "Làm mới trang");
    
    static void executeRefresh(std::function<void()> callback);

private:
    PageRefresher() = delete;
};

#endif // PAGEREFRESHER_H