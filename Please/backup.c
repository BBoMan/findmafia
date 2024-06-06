    if (temp != NULL && temp->pageNum == pageNum) {    //새로 들어온 pageNum이 head에 있을 경우, hit에 해당함
        if(temp->next != NULL) {                       //frame에 head만 있지 않을 경우
            *head = temp->next;                        //head를 다음 값으로 설정
            temp->next = NULL;

            while (last->next != NULL) {               //연결 리스트의 마지막 까지 이동
                last = last->next;                     //last를 한칸씩 이동
            }
            last->next = temp;                         //헤드 노드를 복사한 노드를 연결리스트의 마지막에 추가
            if (strcmp(operator, "write\n") == 0) {    //operator가 write 였다면
                temp->R = 1;                           //R = 1
                temp->M = 1;                           //M = 1
                temp->class = 3;                       //class = 3
            }
            else if (strcmp(operator, "read\n") == 0) {//operator가 read 였다면
                temp->R = 1;                           //R = 1
                temp->class = checkClass(temp->R, temp->M);//class값을 확인함              
            }
        }
        else {                                         //head에 하나만 있을 때
            if (strcmp(operator, "write\n") == 0) {    //operator가 write 였다면
                temp->R = 1;                           //R = 1
                temp->M = 1;                           //M = 1
                temp->class = 3;                       //class = 3
            }
            else if (strcmp(operator, "read\n") == 0) {//operator가 read 였다면
                temp->R = 1;                           //R = 1
                temp->class = checkClass(temp->R, temp->M);//class값을 확인함              
            }
        }
        (*hit)++;                                      //hit를 1 추가
        return;                                        //addNru()함수를 종료
    }
    while (temp != NULL && temp->pageNum != pageNum) { //새로 들어온 pageNum이 중간Node에 있는 경우 or 리스트에 없었을 경우
        prev = temp;                                   //이전 노드에 현재 노드 값 복사
        temp = temp->next;                             //현재 노드에 다음 노드 값 복사
    }
    if (temp != NULL) {                                //해당하는 노드가 있을 경우
        if (temp->next == NULL) {                      //해당 노드가 마지막에 있을 때
            if (strcmp(operator, "write\n") == 0) {    //operator가 write 였다면
                temp->R = 1;                           //R = 1
                temp->M = 1;                           //M = 1
                temp->class = 3;                       //class = 3
            }
            else if (strcmp(operator, "read\n") == 0) {//operator가 read 였다면
                temp->R = 1;                           //R = 1
                temp->class = checkClass(temp->R, temp->M);//class값을 확인함              
            }
            (*hit)++;                                      //hit를 1 추가
            return;                                        //addNru()함수를 종료
        }
        prev->next = temp->next;                       //해당 노드로 접근할 방법을 지움
        temp->next = NULL;
        while (last->next != NULL) {                   //연결 리스트의 마지막까지 이동
            last = last->next;                         //last를 한칸씩 이동
        }
        last->next = temp;                             //해당 노드를 연결 리스트의 마지막에 추가
        temp->next = NULL;                             //마지막 노드의 next = NULL
        if (strcmp(operator, "write\n") == 0) {        //operator가 write 였다면, hit 이므로
            temp->R = 1;                               //R = 1
            temp->M = 1;                               //M = 1
            temp->class = 3;                           //class = 3
        }
        else if (strcmp(operator, "read\n") == 0) {    //operator가 read 였다면
            temp->R = 1;                               //R = 1
            temp->class = checkClass(temp->R, temp->M);//class값을 확인함              
        }
        (*hit)++;                                      //hit를 1 추가
        return;                                        //addNru()함수를 종료
    }









    