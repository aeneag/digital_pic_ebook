/*
* Created by Aen on 2021/7/19.
*/
#include <config.h>
#include <page_manager.h>

static T_Page_action g_t_explore_page_action = {
        .name          = "explore",
        .Run           = ExplorePageRun,
        .GetInputEvent = ExplorePageGetInputEvent,
        .Prepare       = ExplorePagePrepare;
};

static void Explore_page_run(void)
{
    /* 1. 显示页面 */

    /* 2. 创建Prepare线程 */

    /* 3. 调用GetInputEvent获得输入事件，进而处理 */
    while (1)
    {
        InputEvent = ExplorePageGetInputEvent();
        switch (InputEvent)
        {
            case "向上":
            {
                /* 判断是否已经是顶层 */
                if (isTopLevel)
                    return 0;
                else
                {
                    /* 显示上一个目录的页面 */
                }
                break;
            }
            case "选择":
            {
                if (isSelectDir)
                {
                    /* 显示下一级目录 */
                }
                else
                {
                    /* 保存当前页面 */
                    StorePage();
                    Page("browse")->Run();
                    /* 恢复之前的页面 */
                    RestorePage();
                }
                break;
            }
            case "下页":
            {
                /* 显示下一页 */
                break;
            }

            case "上页":
            {
                /* 显示上一页 */
                break;
            }
        }
    }
}


int Explore_page_init(void)
{
    return Register_page_action(&g_t_explore_page_action);
}



