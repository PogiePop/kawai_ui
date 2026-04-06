I would have been building this ui-project with opengl six months after last weekend


UI事件设计->分为window和UI两个部分
window->SizeEvent
      ->CloseEvent
      ->MouseEvent->MouseMoveEvent
                  ->MouseScrollEvent
                  ->MouseButtonEvent->MouseButtonPressEvent
                                    ->MouseButtonReleaseEvent
      ->KeyBoardEvent->KeyPressEvent
                     ->KeyReleaseEvent
                     ->KeyRepeatEvent
                     ->KeyInputEvent
UI->SizeEvent
  ->DestoryEvent
  ->InitEvent
  ->FocusEvent(hover)->MouseEvent->MouseMoveEvent
                                 ->MouseScorllEvent
                                 ->MouseButtonEvent->MouseButtonPressEvent
                                                   ->MouseButtonReleaseEvent
                     ->KeyBoardEvent->KeyInputEvent
  ->other ChildSelfEvent
