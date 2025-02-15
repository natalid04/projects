public class SOLL {
    Node head;
    Node tail;
    Node asc_head;
    Node desc_head;
    int size;

    SOLL(){
        head = null;
        tail = null;
        asc_head = null;
        desc_head = null;
        size = 0;
    }

    public void pushback(int val){
        Node newNode = new Node(val);
        if(tail == null){
            head = newNode;
            tail = newNode;
            asc_head = newNode;
            desc_head = newNode;
            size++;
            return;
        }

        tail.next = newNode;
        newNode.prev = tail;
        tail = newNode;
        updateSort(newNode);
        size++;
    }

    public Node popback(){
        if(head == null){
            return null;
        }

        Node temp = tail;
        tail = tail.prev;
        tail.next = null;
        temp.prev = null;
        updateSortDel(temp);
        size--;
        return temp;
    }

    public void pushfront(int val){
        Node newNode = new Node(val);
        if(head == null){
            head = newNode;
            tail = newNode;
            asc_head = newNode;
            desc_head = newNode;
            size++;
            return;
        }

        newNode.next = head;
        head.prev = newNode;
        head = newNode;
        updateSort(newNode);
        size++;
    }

    public Node popfront(){
        if(tail == null){
            return null;
        }

        Node temp = head;
        head = head.next;
        head.prev = null;
        temp.next = null;
        updateSortDel(temp);
        size--;
        return temp;
    }

    public void insert(int val, int pos){
        Node newNode = new Node(val);
        if(pos == 0){
            pushfront(val);
        }else if(pos == size){
            pushback(val);
        }else if(pos > size){
            throw new IndexOutOfBoundsException("pos is greater than size");
        }

        Node current = head.next;
        while(--pos > 0){
            current = current.next;
        }

        newNode.prev = current;
        newNode.next = current.next;
        current.next.prev = newNode;
        current.next = newNode;
        updateSort(newNode);
        size++;
    }

    public void insert(int val, Node prev){
        if(prev == null){
            throw new IllegalArgumentException("prev is null");
        }

        Node newNode = new Node(val, prev.next, prev);
        prev.next.prev = newNode;
        prev.next = newNode;
        updateSort(newNode);
        size++;
    }

    public void delete(int val){
        if(!search(val)) {
            System.out.println("not found");
            return;
        }

        if(tail == head){
            head = null;
            tail = null;
            asc_head = null;
            desc_head = null;
            size--;
            return;
        }

        Node current = get(val);
        if(current == head){
            popfront();
            return;
        }

        if(current == tail){
            popback();
            return;
        }

        current.prev.next = current.next;
        System.out.println(current.next);
        current.next.prev = current.prev;
        current.next = null;
        current.prev = null;
        updateSortDel(current);
        size--;
    }

    public void print(){
        System.out.print("Natural Order: ");
        Node current = head;
        while(current != null){
            System.out.print(current + ", ");
            current = current.next;
        }
        System.out.println();

        System.out.print("Reverse Order: ");
        current = tail;
        while(current != null){
            System.out.print(current + ", ");
            current = current.prev;
        }
        System.out.println();

        System.out.print("Ascending Order: ");
        current = asc_head;
        while(current != null){
            System.out.print(current + ", ");
            current = current.ascending;
        }
        System.out.println();

        System.out.print("Descending Order: ");
        current = desc_head;
        while(current != null){
            System.out.print(current + ", ");
            current = current.descending;
        }
        System.out.println();
    }

    public Node get(int val){
        if(!search(val)) throw new IllegalArgumentException("not found");

        Node current = head;
        while(current != null){
            if(current.data == val){
                return current;
            }
            current = current.next;
        }

        return null;
    }

    public boolean search(int val){
        Node current = head;
        while(current != null){
            if(current.data == val){
                return true;
            }
            current = current.next;
        }
        return false;
    }


    private void updateSort(Node node){
        if(asc_head.data > node.data){
            node.ascending = asc_head;
            asc_head.descending = node;
            asc_head = node;
            return;
        }

        if(desc_head.data < node.data){
            desc_head.ascending = node;
            node.descending = desc_head;
            desc_head = node;
            return;
        }

        Node temp = asc_head.ascending;
        while(temp.data < node.data){
            temp = temp.ascending;
        }

        temp.descending.ascending = node;
        node.ascending = temp;
        node.descending = temp.descending;
        temp.descending = node;
    }

    private void updateSortDel(Node node){
        if(asc_head == desc_head){
            asc_head = null;
            desc_head = null;
        }
        node.descending.ascending = node.ascending;
        node.ascending.descending = node.descending;
        node.descending = null;
        node.ascending = null;
    }


     static class Node {
        int data;
        Node next;
        Node prev;
        Node ascending;
        Node descending;

        Node(int data) {
            this.data = data;
            this.next = null;
            this.prev = null;
            this.ascending = null;
            this.descending = null;
        }

        Node(int data, Node next, Node prev) {
            this.data = data;
            this.next = next;
            this.prev = prev;
            this.ascending = null;
            this.descending = null;
        }

        public String toString() {
            return "" + data;
        }
    }
}


