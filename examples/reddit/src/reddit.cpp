
#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/ListView.h>
#include <bdn/RowView.h>
#include <bdn/Button.h>
#include <bdn/TextView.h>
#include <bdn/ScrollView.h>
#include <bdn/TextField.h>
#include <bdn/Toggle.h>
#include <bdn/Switch.h>
#include <bdn/Checkbox.h>

#include <bdn/appInit.h>
#include <bdn/UiAppControllerBase.h>

#include <bdn/property/Property.h>

#include <bdn/HTTP.h>
#include <bdn/HTTPRequest.h>
#include <bdn/HTTPResponse.h>

#include <nlohmann/json.hpp>

#include <iostream>

using namespace bdn;

using namespace nlohmann;

class RedditPost
{
public:
    RedditPost() = default;
    RedditPost(String title_) {
        title = title_;
    }
    Property<String> title;
};

class RedditStore
{
public:
    void fetchPosts(std::function<void()> doneHandler)
    {
        auto response = net::http::request({"https://www.reddit.com/hot.json", [this, doneHandler](auto r) {
                                                json j = json::parse(r->data);

                                                for (auto child : j["data"]["children"]) {
                                                    auto post = std::make_shared<RedditPost>();
                                                    post->title = child["data"]["title"];
                                                    posts.push_back(post);
                                                }
            
                                                doneHandler();
                                            }});
    }

    std::vector<std::shared_ptr<RedditPost>> posts;
};

class RedditListViewDataSource : public ListViewDataSource
{
public:
    RedditListViewDataSource(std::shared_ptr<RedditStore> store)
    {
        _store = store;
    }
    
    size_t numberOfRows() override
    {
        return _store->posts.size();
    }
    
    String labelTextForRowIndex(size_t rowIndex) override
    {
        return _store->posts.at(rowIndex)->title;
    }

private:
    std::shared_ptr<RedditStore> _store;
};

class MainViewController : public Base
{
  public:
    MainViewController()
    {
        _window = std::make_shared<Window>();
        _window->title = "UI Demo";

        auto mainColumn = std::make_shared<ColumnView>();
        mainColumn->preferredSizeMinimum = Size(250, 0);

        addControlWithHeading("Switch", std::make_shared<Switch>(), mainColumn, true);
        addControlWithHeading("Checkbox", std::make_shared<Checkbox>(), mainColumn, true);

        auto btn = std::make_shared<Button>();
        btn->label = "Button";
        addControlWithHeading("Button", btn, mainColumn, true);

        std::shared_ptr<TextField> textFieldCtrl = std::make_shared<TextField>();

        addControlWithHeading("Text Field", textFieldCtrl, mainColumn, true);
        textFieldCtrl->preferredSizeMinimum = Size(250, 0);

        auto textScrollView = std::make_shared<ScrollView>();

        // limit the maximum size. We simply want the scroll view to fill the
        // available width and have a height of 100 dips.
        textScrollView->preferredSizeMinimum = Size(0, 186);
        textScrollView->preferredSizeMaximum = Size(0, 186);

        auto scrolledTextView = std::make_shared<TextView>();
        scrolledTextView->text = "The user interface (UI), in the industrial design field of "
                                 "human–computer interaction, is the space where interactions "
                                 "between humans and machines occur. The goal of this interaction "
                                 "is to allow effective operation and control of the machine from "
                                 "the human end, whilst the machine simultaneously feeds back "
                                 "information that aids the operators' decision-making process. "
                                 "Examples of this broad concept of user interfaces include the "
                                 "interactive aspects of computer operating systems, hand tools, "
                                 "heavy machinery operator controls, and process controls. The "
                                 "design considerations applicable when creating user interfaces "
                                 "are related to or involve such disciplines as ergonomics and "
                                 "psychology.\n\n"
                                 "Generally, the goal of user interface design is to produce a user "
                                 "interface which makes it easy, efficient, and enjoyable "
                                 "(user-friendly) to operate a machine in the way which produces "
                                 "the desired result. This generally means that the operator needs "
                                 "to provide minimal input to achieve the desired output, and also "
                                 "that the machine minimizes undesired outputs to the human.\n\n"
                                 "User interfaces are composed of one or more layers including a "
                                 "human-machine interface (HMI) interfaces machines with physical "
                                 "input hardware such a keyboards, mice, game pads and output "
                                 "hardware such as computer monitors, speakers, and printers. A "
                                 "device that implements a HMI is called a human interface device "
                                 "(HID). Other terms for human-machine interfaces are man–machine "
                                 "interface (MMI) and when the machine in question is a computer "
                                 "human–computer interface. Additional UI layers may interact with "
                                 "one or more human sense, including: tactile UI (touch), visual UI "
                                 "(sight), auditory UI (sound), olfactory UI (smell), equilibrial "
                                 "UI (balance), and gustatory UI (taste).\n\n"
                                 "Composite user interfaces (CUI) are UIs that interact with two or "
                                 "more senses. The most common CUI is a graphical user interface "
                                 "(GUI), which is composed of a tactile UI and a visual UI capable "
                                 "of displaying graphics. When sound is added to a GUI it becomes a "
                                 "multimedia user interface (MUI). There are three broad categories "
                                 "of CUI: standard, virtual and augmented. Standard composite user "
                                 "interfaces use standard human interface devices like keyboards, "
                                 "mice, and computer monitors. When the CUI blocks out the real "
                                 "world to create a virtual reality, the CUI is virtual and uses a "
                                 "virtual reality interface. When the CUI does not block out the "
                                 "real world and creates augmented reality, the CUI is augmented "
                                 "and uses an augmented reality interface. When a UI interacts with "
                                 "all human senses, it is called a qualia interface, named after "
                                 "the theory of qualia. CUI may also be classified by how many "
                                 "senses they interact with as either an X-sense virtual reality "
                                 "interface or X-sense augmented reality interface, where X is the "
                                 "number of senses interfaced with. For example, a Smell-O-Vision "
                                 "is a 3-sense (3S) Standard CUI with visual display, sound and "
                                 "smells; when virtual reality interfaces interface with smells and "
                                 "touch it is said to be a 4-sense (4S) virtual reality interface; "
                                 "and when augmented reality interfaces interface with smells and "
                                 "touch it is said to be a 4-sense (4S) augmented reality "
                                 "interface.\n\nThis text uses material from the Wikipedia article "
                                 "https://en.wikipedia.org/wiki/User_interface";
        textScrollView->setContentView(scrolledTextView);

        addControlWithHeading("Scrolling multiline text", textScrollView, mainColumn, false);

        
        
        auto store = std::make_shared<RedditStore>();
        store->posts.push_back(std::make_shared<RedditPost>("Test"));
        store->posts.push_back(std::make_shared<RedditPost>("Test2"));
        store->posts.push_back(std::make_shared<RedditPost>("Test3"));
        store->posts.push_back(std::make_shared<RedditPost>("Test4"));
        store->posts.push_back(std::make_shared<RedditPost>("Test5"));
        store->posts.push_back(std::make_shared<RedditPost>("Test6"));
        store->posts.push_back(std::make_shared<RedditPost>("Test7"));
        store->posts.push_back(std::make_shared<RedditPost>("Test8"));
        store->posts.push_back(std::make_shared<RedditPost>("Test9"));
        store->posts.push_back(std::make_shared<RedditPost>("Test10"));
        store->posts.push_back(std::make_shared<RedditPost>("Test11"));
        store->posts.push_back(std::make_shared<RedditPost>("Test12"));
        store->posts.push_back(std::make_shared<RedditPost>("Test13"));
        store->posts.push_back(std::make_shared<RedditPost>("Test14"));
        store->posts.push_back(std::make_shared<RedditPost>("Test15"));
        store->posts.push_back(std::make_shared<RedditPost>("Test16"));
        store->posts.push_back(std::make_shared<RedditPost>("Test17"));
        store->posts.push_back(std::make_shared<RedditPost>("Test18"));
        _dataSource = std::make_shared<RedditListViewDataSource>(store);

        auto listView = std::make_shared<ListView>();
        listView->dataSource = _dataSource;

        store->fetchPosts([listView]() {
            listView->reloadData();
        });
        
        listView->horizontalAlignment = View::HorizontalAlignment::expand;
        listView->verticalAlignment = View::VerticalAlignment::expand;
        listView->preferredSizeMinimum = Size(100, 200);
        listView->margin = UiMargin(15, 15, 15, 15);

        mainColumn->addChildView(listView);

        auto mainScrollView = std::make_shared<ScrollView>();
        mainScrollView->setContentView(mainColumn);

        _window->setContentView(mainScrollView);

        _window->requestAutoSize();
        _window->requestCenter();

        _window->visible = true;
    }

    void addControlWithHeading(String headingText, std::shared_ptr<View> control,
                               std::shared_ptr<ContainerView> container, bool single)
    {
        auto header = std::make_shared<TextView>();
        header->text = headingText;
        header->horizontalAlignment = View::HorizontalAlignment::left;

        if (single) {
            control->horizontalAlignment = View::HorizontalAlignment::right;
            control->verticalAlignment = View::VerticalAlignment::middle;
            header->verticalAlignment = View::VerticalAlignment::middle;
            auto row = std::make_shared<RowView>();
            row->margin = UiMargin(15, 15, 15, 15);
            row->horizontalAlignment = View::HorizontalAlignment::expand;
            header->horizontalAlignment = View::HorizontalAlignment::expand;
            row->addChildView(header);
            row->addChildView(control);
            container->addChildView(row);
        } else {
            header->margin = UiMargin(15 + 6, 15, 18, 15);
            control->margin = UiMargin(2, 15, 15, 15);
            control->horizontalAlignment = View::HorizontalAlignment::expand;

            container->addChildView(header);
            container->addChildView(control);
        }
    }

  protected:
    std::shared_ptr<Window> _window;
    std::shared_ptr<RedditListViewDataSource> _dataSource;
};

class AppController : public UiAppControllerBase
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override
    {
        _mainViewController = std::make_shared<MainViewController>();
    }

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};

BDN_APP_INIT(AppController)