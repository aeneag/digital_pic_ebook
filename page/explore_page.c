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
    /* 1. ��ʾҳ�� */

    /* 2. ����Prepare�߳� */

    /* 3. ����GetInputEvent��������¼����������� */
    while (1)
    {
        InputEvent = ExplorePageGetInputEvent();
        switch (InputEvent)
        {
            case "����":
            {
                /* �ж��Ƿ��Ѿ��Ƕ��� */
                if (isTopLevel)
                    return 0;
                else
                {
                    /* ��ʾ��һ��Ŀ¼��ҳ�� */
                }
                break;
            }
            case "ѡ��":
            {
                if (isSelectDir)
                {
                    /* ��ʾ��һ��Ŀ¼ */
                }
                else
                {
                    /* ���浱ǰҳ�� */
                    StorePage();
                    Page("browse")->Run();
                    /* �ָ�֮ǰ��ҳ�� */
                    RestorePage();
                }
                break;
            }
            case "��ҳ":
            {
                /* ��ʾ��һҳ */
                break;
            }

            case "��ҳ":
            {
                /* ��ʾ��һҳ */
                break;
            }
        }
    }
}


int Explore_page_init(void)
{
    return Register_page_action(&g_t_explore_page_action);
}



