//
// Created by Sayama on 09/01/2026.
//

#pragma once

namespace Todo
{
    template<std::copyable T>
    class LockFreeStack
    {
        struct Node
        {
            Node(const T& data) : data(std::make_shared<T>(data)) {}
            std::shared_ptr<T> data;
            Node* next{nullptr};
        };
    public:
        void push(const T& data)
        {
            Node* node = new Node(data);
            node->next = head.load(std::memory_order_seq_cst);
            while (!head.compare_exchange_weak(node->next, node, std::memory_order_seq_cst));
        }

        std::shared_ptr<T> pop()
        {
            thread_in_pop.fetch_add(1, std::memory_order_seq_cst);
            Node* oldHead = head.load(std::memory_order_seq_cst);
            while (oldHead && head.compare_exchange_weak(oldHead, oldHead->next, std::memory_order_seq_cst));
            std::shared_ptr<T> result;
            if (oldHead)
            {
                std::swap(result, oldHead->data);
            }
            try_reclaim(oldHead); // 'thread_in_pop' will be decreased here.
            return result;
        }
    private:
        static inline void delete_nodes(Node* nodes)
        {
            while (nodes)
            {
                Node* next = nodes->next;
                delete nodes;
                nodes = next;
            }
        }

        void try_reclaim(Node* node)
        {
            if (thread_in_pop.load(std::memory_order_seq_cst) == 1)
            {
                Node* nodes_to_delete = to_be_deleted.exchange(nullptr, std::memory_order_seq_cst);
                if (thread_in_pop.fetch_sub(1, std::memory_order_seq_cst) == 1)
                {
                    delete_nodes(nodes_to_delete);
                }
                else
                {
                    chain_pending_nodes(nodes_to_delete);
                }
                delete node;
            }
            else
            {
                chain_pending_nodes(node);
                thread_in_pop.fetch_sub(1, std::memory_order_seq_cst);
            }
        }

        void chain_pending_nodes(Node* nodes)
        {
            Node* last = nodes;
            while (const Node* next = last->next)
            {
                last = next;
            }
            chain_pending_nodes(nodes, last);
        }

        void chain_pending_nodes(Node* first, Node* last)
        {
            last->next = to_be_deleted.load(std::memory_order_seq_cst);
            while (!to_be_deleted.compare_exchange_weak(last->next, first));
        }

        void chain_pending_node(Node* node)
        {
            chain_pending_nodes(node, node);
        }

    private:
        std::atomic<Node*> head;
        std::atomic<uint32_t> thread_in_pop;
        std::atomic<Node*> to_be_deleted;

    };
}