// courtesy of https://gist.github.com/PsychedelicShayna/9cc2166ed5598456f88d78620bc64864
// slightly tweaked and with arguments removed since i won't be using them
#ifndef GENERICEVENTFILTER_H
#define GENERICEVENTFILTER_H
#include <QObject>

class GenericEventFilter : public QObject
{
public:
    template<typename Func>
    GenericEventFilter(QObject* parent, Func&& function)
        : lambdaContainer(new LambdaContainer<Func>(std::forward<Func>(function))), parent(parent) {}

    ~GenericEventFilter() override
    {
        if (lambdaContainer)
            delete lambdaContainer;
    }

    bool eventFilter(QObject* watched, QEvent* event) override
    {
        return lambdaContainer->callLambdaFunction(parent, watched, event);
    }
private:
    QObject* parent;

    struct AbstractLambdaContainer
    {
        virtual bool callLambdaFunction(QObject*, QObject*, QEvent*) = 0;
        virtual ~AbstractLambdaContainer() = default;
    } *lambdaContainer;

    template<typename Func>
    class LambdaContainer : public AbstractLambdaContainer
    {
    public:
        LambdaContainer(Func&& function) : function(std::forward<Func>(function)) {}

        bool callLambdaFunction(QObject* parent, QObject* watched, QEvent* event) override
        {
            return std::apply(function, std::make_tuple(parent, watched, event));
        }
    private:
        Func function;
    };
};

#endif // GENERICEVENTFILTER_H
