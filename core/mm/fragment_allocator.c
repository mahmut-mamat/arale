#include "page.h"
#include "mmzone.h"
#include "list.h"
#include "mm.h"

//#define PAGE_SIZE 4096
//#define ZONE_MEMORY 1024*1024*7l

mm_zone high_memory_zone;
mm_page total_page;
struct list_head free_page_list;
struct rb_root used_page_root;

//because vmalloc can use uncontinous memory,
//so we can alloc 4K page 
typedef struct fragment_node
{
    struct list_head ll;
    struct rb_node rb;
    mm_page page;
} fragment_node;

static void rb_insert_used_node(fragment_node *node, struct rb_root *root)
{
    struct rb_node **new = &root->rb_node, *parent = NULL;
    addr_t start_pa = node->page.start_pa;

    while (*new) 
    {
        parent = *new;
        if (start_pa < rb_entry(parent, fragment_node, rb)->page.start_pa)
            new = &parent->rb_left;
        else
            new = &parent->rb_right;
    }

    rb_link_node(&node->rb, parent, new);
    rb_insert_color(&node->rb, root);
}

static fragment_node *rb_find_node(addr_t start_addr,struct rb_root *root)
{
    struct rb_node **new = &root->rb_node, *parent = NULL;
    addr_t start_pa = start_addr;

    while (*new) 
    {
        parent = *new;
        fragment_node *node = rb_entry(parent, fragment_node, rb);
        addr_t pa = node->page.start_pa;

        if (start_pa == pa)
        {
            //new = &parent->rb_left;
            return node;
        }
        else if(start_pa < pa)
        {
            new = &parent->rb_left;
        }
        else if(start_pa > pa)
        {
            new = &parent->rb_right;
        }
    }
    return NULL;
}

static void rb_erase_used_fragment(fragment_node *node,struct rb_root *root) 
{
    rb_erase(&node->rb,root);
}
void fragment_allocator_init(addr_t start_addr,uint32_t size)
{

    if(size < PAGE_SIZE) 
    {
        //kprintf("PAGE SIZE error!!!!! \n");
        return;
    }

    INIT_LIST_HEAD(&free_page_list);
    //INIT_LIST_HEAD(&used_page_list);

    total_page.start_pa = start_addr;
    total_page.size = size;
}

void* get_fragment_page(uint32_t size) 
{
    fragment_node *frag = NULL;

    if(!list_empty(&free_page_list))
    {
        frag = list_entry(free_page_list.next,fragment_node,ll);
        list_del(&frag->ll);
    } else {
        //we should divide a new page to free list;
        frag = kmalloc(sizeof(fragment_node));

        uint32_t divide_size = PAGE_SIZE;
        //kprintf("wangsl,divide_size is %d \n",divide_size);

        if(total_page.size >= divide_size)
        {
            //kprintf("get:start_page is  0x%x,divide_size is %x \n",total_page,divide_size);
            frag->page.start_pa = (uint64_t)total_page.start_pa;
            //kprintf("frag->page start pa is %llx \n",frag->page.start_pa);
            frag->page.size = divide_size;

            total_page.start_pa = (uint64_t)total_page.start_pa + divide_size;
            total_page.size -=divide_size;
            //kprintf("wangsl,divide a new memory \n");
            //kprintf("get:total_page is  0x%x,size is %x \n",total_page,total_page->size);
        }
    }

    if(frag != NULL) 
    {
        //kprintf("get_fragment_page add rbtree %x \n",frag->page.start_pa);
        rb_insert_used_node(frag,&used_page_root);

        //char *tt = new_page;
        //kprintf("tt is  %x \n",(tt + sizeof(mm_page)));
        return (void *)frag->page.start_pa;
    }

    return NULL;
}

int free_fragment_page(addr_t page_addr)
{
    //mm_page *del_page = page_addr - sizeof(mm_page);
    //kprintf("page_addr is %x,sizeof(mm_page) is %x \n",page_addr,sizeof(mm_page));
    page_addr &= ~0x0FFF;
    //kprintf("page_addr is %x \n",page_addr);
    fragment_node *node = rb_find_node(page_addr,&used_page_root);
    if(node == NULL) 
    {
        kprintf("wangsl error!!!!!\n");
        return;
    }

    //kprintf("free info size is %d,start pa is %x \n",node->page.size,node->page.start_pa);
    rb_erase_used_fragment(node,&used_page_root);
    //list_del(&del_page->ll);
    list_add(&node->ll,&free_page_list);

    return 0;
}

void fragment_allocator_dump()
{
    struct list_head *p;
    int index = 0;
    kprintf("============free page============\n");
    list_for_each(p,&free_page_list) {
        mm_page *page = list_entry(p,mm_page,ll);
        kprintf("   %d: page size is %x \n",index,page->size);
        kprintf("   %d: page start_pa is %x,addr is %x \n",index,page->start_pa,page);
        index++;
    }

    //kprintf("============used page============\n");
    //list_for_each(p,&used_page_list) {
    //    mm_page *page = list_entry(p,mm_page,ll);
    //    kprintf("   %d: page size is %x \n",index,page->size);
    //    kprintf("   %d: page start_pa is %x,addr is %x \n",index,page->start_pa,page);
    //    index++;
    //}
}

#if 0
int main()
{
    data = malloc(1024*1024*7l);
    fk_allocator_init(data,1024*1024*7l);

    page1 = get_4k_page();
    
    page2 = get_4k_page();
    page3 = get_4k_page();
    page4 = get_4k_page();

   // dump();
   kprintf("mm page is %d \n",sizeof(mm_page));
   free_4k_page(page1);
   free_4k_page(page2);
   free_4k_page(page3);
   free_4k_page(page4);
   dump();

}
#endif



