# A Window

W window is a programming construct that:

* Occupies a certain portion of the screen
* May or may not be visible at a given moment
* Knows how to draw itself
* Responds to events from the user or OS

The usual idea of a window being a rectangle with a title bar and maximize/minimize/close buttons is misleading because a button that you see in a window, by definition, is a window. All UI controls in Windows (the OS) are. A basic difference of a window that is a UI control from an _application window_ is that they don't exist by themselves, but positioned relative to the application window. Windows have parent/child or owner/owned relationships. A button is a child window, a modal dialog window is owned. The owned window's lifetime is the owner's and always appears in front of the owner and hidden when the owner is. 

## Window Handles

The OS maintains a table of windows and refers to them by a value called _handle_. The handle _HWND_ (pronounced "aitch-wind") is an opaque type. The _CreateWindow()_ and _CreateWindowEx()_ functions return a HWND, and the handle is used to perform some operation on a window using functions like _MoveWindow()_.

## Screen and Window Coordinates

Coordinates are measured in device-independent pixels (DIP), relative to the screen, relative to the window or relative to the window's client area. The origin is the top left and windows are positioned relative to the screen and drawn into using client coordinates.

## WinMain: The Application Entry Point

The function signature is:

    int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);

_wWinMain_ accepts command-line arguments as Unicode while WinMain reads it in as an ANSI string. _pCmdLine_ contains the command-line arguments, _nCmdShow_ determines whether the main application window will be minimized, maximized or normal. _hPrevInstance_ is obsolete now, a remnant from 16-bit Windows. _hInstance_ is a handle to an instance or handle to a module. The OS uses this value to identify the executable (EXE file) when it is loaded in memory. It is needed for certain Windows functions like loading icons or bitmaps.

The Microsoft C runtime library (CRT) provides an implementation of _main_ that calls either _WinMain_ or _wWinMain_. In addition the CRT calls any static initializers. Although the linker can be told to use a different entry point, such things should be avoided because that would amount to skipping CRT's initialization code leading to unpredictable results.

## Window Classes

A _window class_ defines a set of behaviors that several windows might have in common. Data that is unique for each window is called _instance data_. Every window must be associated with a window class. This is not a C++ class but a data structure used internally by the OS. Classes are registered with the system at runtime using the _RegisterClass()_ function.

Of the many fields of the structure the three shown in the code must be filled in: a window procedure, the handle to the application instance and a string that identifies the class (_lpfnWndProc_, _hInstance_, _lpszClassName_). Class names are local to the current process, so the name only needs to be unique within the process. 

## Creating a Window

To create a new instance of a window we call _CreateWindowEx()_ with a long list of parameters. A few things to note about them is that the first parameter lets you specify optional behaviors (like transparent windows), the window text is used in the title bar (or in different ways by different types of windows), the style is a set of bitwise-OR flags (WS_OVERLAPPEDWINDOW has a title bar, border, system menu and minimize/maximize buttons; the most common style for a top-level application window), the position and size are set to default values (CW_USEDEFAULT), the NULLs are parent window, menu for the window and a _void*_ pointer to additional data, and, the hInstance is the instance handle received by _wWinMain()_.

The function call returns a handle tot he new window or zero if the function fails.

Subsequently, _ShowWindow()_ is called with the _nCmdShow_ parameter which is used to minimize or maximize a window. The OS passes this value tot he program.

## Window Messages

Events from the user and the OS are passed on to the window using messages. As events may occur at any time, in almost any order, the flow of execution can not be predicted. So Windows uses a message passing model. A message is simply a numeric code that designates a particular event. To pass a message to a window the OS calls its window procedure.

For each thread that creates a window, the OS creates a queue for windows messages. This queue holds messages for all the windows that are created on that thread. The _GetMessage_ is used to pull messages out of the queue. If the queue is empty this function blocks until another message is queued. Even though _GetMessage_ blocks, your program will not become unresponsive because there is nothing for it to do when there are no messages. Additional threads can be spawned to do background processing while GetMessage waits.

The MSG structure contains information about a message but you will almost never examine this structure directly. Instead messages are passed to _TranslateMessage_ and _DispatchMessage_. TranslateMessage translates keystrokes into characters. DispatchMessage tells the OS to call the window procedure of the target window of the message.

To exit the application (or quit the message processing loop), call _PostQuitMessage_ which puts a WM\_QUIT message on the queue and this in turn causes GetMessage to return zero. One interesting result of this behavior is that your window procedure never receives a WM\_QUIT message, and therefore you need not handle this message in the switch-case block in your window procedure.

Messages are of two kinds: _Posted_messages are those that go on the message queue and is dispatched through the message loop. _Sent_ messages skip the queue and the OS calls the window procedure directly.

## The Window Procedure

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

We write this out as a switch-case structure that performs actions for each kind of message we want to handle. For a uMsg, wParam and lParam which are word-size integers, carry more information. _DefWindowProc_ must be called at the end to take care of all other messages.

While a window procedure executes, it blocks any other messages for windows created on the same thread. Therefore, lengthy computations must be avoided in the window procedure. Something you wouldn't want to do is open a TCP connection and wait for the server response from inside the window procedure. In stead, you should move that to another thread using one of the multitasking facilities built into Windows:

* Create a new thread.
* Use a thread pool.
* Use asynchronous I/O calls.
* Use asynchronous procedure calls.

## Painting the Window and the WM\_PAINT Message

Showing something in the client area of the window is termed _painting the window_. Sometimes your program will want to update a region of the window, or the OS will want you to. In either case your or the OS sends your window a WM\_PAINT message which tells you the _update region_ or the portion to be painted.

The painting has to happen between a call to _BeginPaint_ and _EndPaint_. BeginPaint fills a structure called _PAINTSTRUCT_ with information on the repaint request. One of which is a _RECT_ structure specifying a rectangular _update region_ (defined relative to the client area). At this point you have two options; paint the entire client area regardless of the size of the _update region_ or, more efficiently, paint just the _update region_. It is less code to write in the former case, but more efficient in the latter case.

EndPaint, called last, after your painting logic, clears the update region, which in turn signals to Windows that the window has completed painting itself and that it doesn't need to send another WM\_PAINT until something changes.

On the first WM\_PAINT message the entire client area needs to be painted. The _update region's_ size is the entire client rectangle. On subsequent WM\_PAINT messages, PAINTSTRUCT's _rcPaint_ field may contain a smaller rectangle.

The recommended pattern is to skip handling WM\_CLOSE and leaving it to _DefWindowProc_ to catch it and call _DestroyWindow_, or, catch WM\_CLOSE yourself, do something in there and call _DestroyWindow_. Then, in WM\_DESTROY (which you handle either way), call _PostQuitMessage_. Then, as you know, WM\_QUIT is sent to the window, which you don't need to handle.