PROGRAM DYNAMIC
    IMPLICIT NONE
    TYPE NODE
        INTEGER :: value
        TYPE(NODE), POINTER :: next
    END TYPE NODE

    TYPE(NODE), POINTER :: head
    TYPE(NODE), POINTER :: tail
    NULLIFY(head)
    NULLIFY(tail)
    CALL insertList(head, tail, 0)
    CALL insertList(head, tail, 1)

    CALL printList(head, tail)


CONTAINS

    SUBROUTINE insertList(head, tail, value)
        TYPE(node), POINTER :: head
        TYPE(node), POINTER :: tail
        INTEGER, INTENT(IN) :: value
        TYPE(node), POINTER :: new
        ALLOCATE(new)
        NULLIFY(new % next)
        new % value = value
        IF (.NOT. ASSOCIATED(head)) THEN
            head => new
            tail => new
        ELSE
            tail % next => new
            tail => new
        END IF
    END SUBROUTINE insertList

    SUBROUTINE printList(head, tail)
        TYPE(node), POINTER :: head
        TYPE(node), POINTER :: tail
        TYPE(node), POINTER :: now
        now => head
        DO
            IF (.NOT. ASSOCIATED(now)) THEN
                RETURN
            END IF
            PRINT *, now % value
            now => now % next
        END DO
    END SUBROUTINE printList
    
END PROGRAM DYNAMIC
